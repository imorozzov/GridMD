/****************************************************************************
 * \file plinkshell.cpp
 * Implementation of the class gmShellLibssh2
****************************************************************************/

#include <stdio.h>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
//#include <boost/asio/io_service.hpp>
//#include <boost/asio/ip/tcp.hpp>
#include <boost/utility.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <boost/bind/protect.hpp>
#include <sstream>
#include <fstream>

#include <gmd/utils.h>
#include <gmd/filename.h>
#include <gmd/filefn.h>

#include "jobmngr/libssh2shell.h"

//#include "libssh2_config.h"
#include <libssh2.h>


#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

namespace
{
#ifdef WIN32
class using_WSA : public boost::noncopyable
{
private:
    WSADATA wsadata;
    struct tag_private {};
public:
    using_WSA(tag_private)
    {
        WSAStartup(MAKEWORD(2,0), &wsadata);
    }
    ~using_WSA()
    {
        WSACleanup() ;
    }
    static boost::shared_ptr<void> instance()
    {
        static boost::shared_ptr<using_WSA> ret= boost::make_shared<using_WSA>( tag_private() );
        return ret;
    }
};
#else
class using_WSA : public boost::noncopyable
{
private:
    struct tag_private {};
public:
    using_WSA(tag_private) {}
    ~using_WSA() {}
    static boost::shared_ptr<void> instance()
    {
        static boost::shared_ptr<void> ret;
        return ret;
    }
};
#endif
static const boost::shared_ptr<void> using_WSA_ = using_WSA::instance();

class using_libssh2 : public boost::noncopyable
{
    boost::shared_ptr<void> using_WSA_;
    using_libssh2()
        : using_WSA_(using_WSA::instance())
    {
        int err_code=libssh2_init(0);
        if(err_code!=0)
        {
            //TODO:
        }
    }
public:
    ~using_libssh2()
    {
        libssh2_exit();
    }
    static boost::shared_ptr<void> instance()
    {
        static boost::shared_ptr<using_libssh2> ret( new using_libssh2() );
        return ret;
    }
};
static const boost::shared_ptr<void> using_libssh2_ = using_libssh2::instance();

using namespace boost::asio;

// see http://www.libssh2.org/examples/ssh2_exec.html
// see also http://cidebycide.blogspot.ru/2012/08/integrating-libssh2-and-boostasio.html
// see also http://cidebycide.blogspot.pt/p/libssh2-ssh2exec-with-boostasio.html
class SshComm : public boost::noncopyable
{
    typedef SshComm me;


    void throw_error(const char* msg=NULL, int err_no=0) const
    {
        //TODO: remove next line.
        //if(!msg) msg="";
        std::cerr<<"Error:\n" << (msg ? msg : "") <<std::endl;

        if(err_no==0 && ss )
        {
            err_no= libssh2_session_last_errno( ss.get() );
            std::cerr<< " errno: " << err_no <<std::endl;
        }

        //TODO: use common trace engine
        std::string bufer;
        bufer.reserve(1024);

        if(msg) bufer += msg;

        if(ss)
        {
            char* buf=0;
            libssh2_session_last_error( ss.get(), & buf, 0, 0 );
            if(buf)
            {
                if(!bufer.empty()) bufer += "\n";
                bufer += buf;
            }
        }

        if(err_no!=0)
        {
            if(!bufer.empty()) bufer += "\n";
            bufer += boost::format("errno:%1%").bind_arg(1, err_no).str();
        }

        //TODO: remove next line.
        std::cerr<<"Error:\n" << bufer <<std::endl;

        throw std::runtime_error(bufer.c_str());
    }

    //typedef std::runtime_error re;
    static void clean_LIBSSH2_SESSION(LIBSSH2_SESSION*ss)
    {
        if(ss)
        {
            //std::cout<< "Clean LIBSSH2 session: " << ss <<std::endl;
            libssh2_session_disconnect(ss,"Goodbye.");
            libssh2_session_free(ss);
        }
    }
    static boost::shared_ptr<LIBSSH2_SESSION> make_LIBSSH2_SESSION()
    {
        boost::shared_ptr<LIBSSH2_SESSION> ret( libssh2_session_init(), &SshComm::clean_LIBSSH2_SESSION );
        if(!ret)
        {
            //throw_error("Fail to create new LIBSSH2_SESSION.");
        }
        //std::cout<< "Open LIBSSH2 session: " << ret.get() <<std::endl;
        libssh2_session_set_blocking(ret.get(), 0);
        return ret;
    }

    static void clean_LIBSSH2_CHANNEL(LIBSSH2_CHANNEL*ch)
    {
        if(ch)
        {
            //std::cout<< "Clean LIBSSH2 channel: " << ch  <<std::endl;
            libssh2_channel_close(ch);
            libssh2_channel_free(ch);
            //std::cout<< "Clean LIBSSH2 channel ok." <<std::endl;
        }
    }

    //mutable boost::shared_ptr<LIBSSH2_CHANNEL> session_channel;
    boost::shared_ptr<LIBSSH2_CHANNEL> open_LIBSSH2_CHANNEL() const
    {
        //if(session_channel)
        //    return session_channel;

        LIBSSH2_CHANNEL* ch=NULL;
        int rc=0;
        do
        {
            ch=libssh2_channel_open_session(ss.get());
            rc = libssh2_session_last_error(ss.get(), nullptr, nullptr, 0);
        }
        while( ch==NULL && rc== LIBSSH2_ERROR_EAGAIN );

        if( !ch )
            throw_error("Could not open SSH communication channel.");

        boost::shared_ptr<LIBSSH2_CHANNEL> ret ( ch, & SshComm::clean_LIBSSH2_CHANNEL );
        //std::cout<< "Open LIBSSH2 channel: " << ch.get() <<"  (session:) " << ss.get() <<std::endl;
        //session_channel=ch;
        return ret;
    }

    enum { buf_size=1024*4 };
    //template< typename ostream >
    int   read_channel_stream(
        LIBSSH2_CHANNEL* ch,
        std::ostream& response_stream,
        int stream_id)
    {
        char buf[buf_size+1];
        for(int res;;)
        {
            res=libssh2_channel_read_ex(ch, stream_id ,buf , buf_size);
            if(res>0)
            {
                response_stream.write(buf,res);
                response_stream.flush();
            };
            if(res==0)
            {
                return res;
            }
            if( res==LIBSSH2_ERROR_EAGAIN )
            {
                return res;
            }
            if( res<0 && res!= LIBSSH2_ERROR_EAGAIN )
            {
                // TODO: Error (?)
                return res;
            }
        }
    }
    //template< typename ostream >
    int   read_channel_streams(
        LIBSSH2_CHANNEL* ch,
        std::ostream& response_stream_cout,
        std::ostream& response_stream_cerr)
    {
        for(;;)
        {
            int res_cout=read_channel_stream(ch, response_stream_cout, 0);
            if(res_cout<0 && res_cout!=LIBSSH2_ERROR_EAGAIN)
                return res_cout;

            int res_cerr=read_channel_stream(ch, response_stream_cerr, SSH_EXTENDED_DATA_STDERR);
            if(res_cerr<0 && res_cerr!=LIBSSH2_ERROR_EAGAIN)
                return res_cerr;

            if(res_cout>0 || res_cerr>0)
                continue;

            if(res_cout==0 && res_cerr==0)
                return 0;

            assert( ( res_cout==0 && res_cerr==LIBSSH2_ERROR_EAGAIN )
                 || ( res_cout==LIBSSH2_ERROR_EAGAIN && res_cerr==0 )
                 || ( res_cout==LIBSSH2_ERROR_EAGAIN && res_cerr==LIBSSH2_ERROR_EAGAIN )
                 );
            return LIBSSH2_ERROR_EAGAIN;
        }
    }

    enum state { WORK_DONE, STILL_WORKING };
    enum direction { READ =1, WRITE=2, READ_AND_WRITE=READ|WRITE };
    void loopAsyncStep(boost::function<state()> DoWork, direction isRead)
    {
       if (sck.get_io_service().stopped())
           return;
       state st = DoWork();
       if (st == STILL_WORKING)
       {
           //boost::function<void()> DoWork2=boost::bind(&SshComm::loopAsyncStep, this, boost::protect(DoWork), isRead);
           if(isRead&READ)
               sck.async_read_some (boost::asio::null_buffers(), boost::bind(&SshComm::loopAsyncStep, this, boost::protect(DoWork), READ) );
           if(isRead&WRITE)
               sck.async_write_some(boost::asio::null_buffers(), boost::bind(&SshComm::loopAsyncStep, this, boost::protect(DoWork), WRITE) );
       }
       else
       {
           // WORK IS OVER
           sck.get_io_service().stop();
       }
    }
    void loopAsync(direction isRead, boost::function<state()> DoWork )
    {
       io.reset();
       if( isRead & READ )
           sck.async_read_some(boost::asio::null_buffers(),
                               boost::bind(&SshComm::loopAsyncStep, this,
                                           boost::protect(DoWork), READ));
       if( isRead & WRITE )
           sck.async_write_some(boost::asio::null_buffers(),
                                boost::bind(&SshComm::loopAsyncStep, this,
                                            boost::protect(DoWork), WRITE));
       io.run();
    }
//    void loopAsync( direction isRead, state (SshComm::* foo)()  )
//    {
//       loopAsync(isRead, boost::bind(foo, this) );
//    }



    state libssh_code_to_async(int code, const char * err_msg=NULL)
    {
        if(code==0)
            return WORK_DONE;
        if(code==LIBSSH2_ERROR_EAGAIN)
            return STILL_WORKING;
        if(code<0)
            throw_error( err_msg, code );

        return STILL_WORKING;
    }

    boost::function<state()> libssh_code_to_async_( boost::function<int()> foo, const char * err_msg=NULL)
    {
        return boost::bind( &me::libssh_code_to_async,
                            this,
                            boost::bind(foo),
                            err_msg);
    }


    state libssh_code_to_async_(const char * err_msg=NULL)
    {
        return libssh_code_to_async( libssh2_session_last_errno( ss.get() ) , err_msg);
    }


    state async_channel_exec(LIBSSH2_CHANNEL * ch, const char* command)
    {
        return libssh_code_to_async(
                    libssh2_channel_exec(ch, command),
                    "Could not startup the SSH communication session.");
    }


    void ini_session(ip::tcp::endpoint point)
    {
        sck.connect( point );
        loopAsync( WRITE,
                   libssh_code_to_async_(
                       boost::bind( &libssh2_session_startup,
                                    ss.get(), sck.native()),
                       "Could not startup the SSH communication session."
                       )
                   );
        //if( 0 > libssh2_session_startup(ss.get(),sck.native()) )
        //    throw_error("Could not startup the SSH communication session.");

        //libssh2_trace(ss.get(), ~ (int) 0);
    }


public:

    SshComm(
        ip::tcp::endpoint point,
        const char* user,
        const char* pw)
        : io(),
          sck(io, ip::tcp::v4()),
          ss(make_LIBSSH2_SESSION())
    {
        ini_session(point);
        loopAsync( WRITE,
                   libssh_code_to_async_(
                       boost::bind( &libssh2_userauth_password_ex,
                                    ss.get(),
                                    user, strlen(user),
                                    pw,   strlen(pw),
                                    (LIBSSH2_PASSWD_CHANGEREQ_FUNC((*)))NULL),
                       "Access denied."
                       )
                   );
        //if( libssh2_userauth_password(ss.get(),user,pw) )
        //    throw_error("Access denied");
    }
    SshComm(
        ip::tcp::endpoint point,
        const char* user,
        const char* key_file_path,
        const char* pw)
        : io(),
          sck(io, ip::tcp::v4()),
          ss(make_LIBSSH2_SESSION())
    {
        ini_session(point);
        loopAsync( WRITE,
                   libssh_code_to_async_(
                       boost::bind( &libssh2_userauth_publickey_fromfile_ex,
                                    ss.get(), user, strlen(user), (const char*)NULL , key_file_path, pw),
                       "Access denied. (Autentification with key file)."
                       )
                   );
    }


    /// @brief Deallocate memory needed for this object.
    virtual ~SshComm()
    {
        try
        {
            ss.reset();
        }
        catch(std::exception& err)
        {
            // Mute exceptions ...
        }

        try
        {
            if(sck.is_open())
            {
                boost::system::error_code ec;
                sck.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
                sck.close(ec);
            }
        }
        catch(std::exception& err)
        {
            // Mute exceptions ...
        }
    }

    /// @brief Execute a command on the SSH server and expect
    /// a response.
    /// @param cmd[in] The command to execute.
    /// @param response_stream[in,out] The stream to write the
    /// expected response to.
    //template< typename ostream >
    int   execute(
        const char* cmd,
        std::ostream& response_stream_stdout,
        std::ostream& response_stream_stderr)
    {
        boost::shared_ptr<LIBSSH2_CHANNEL> ch = open_LIBSSH2_CHANNEL();
        libssh2_channel_set_blocking(ch.get(),0);

        //loopAsync( WRITE,
        //           boost::bind( & me::async_channel_exec, this, ch.get(), cmd ) );
        loopAsync( WRITE,
                   libssh_code_to_async_(
                       boost::bind( &libssh2_channel_process_startup,
                                    ch.get(),
                                    "exec", sizeof("exec") - 1,
                                    cmd, strlen(cmd)
                                    ),
                       "Could not startup the SSH communication session."
                       )
                   );

//        loopAsync( boost::bind( & me::libssh_code_to_async,
//                                 this,
//                                 boost::bind( &libssh2_channel_process_startup,
//                                              ch.get(),
//                                              "exec", sizeof("exec") - 1,
//                                              cmd, strlen(cmd)
//                                              ),
//                                 "Could not startup the SSH communication session."
//                                 ),
//                    WRITE);

        loopAsync( READ,
                   libssh_code_to_async_(
                       boost::bind( & me::read_channel_streams,
                                    this, ch.get(),
                                    boost::ref(response_stream_stdout),
                                    boost::ref(response_stream_stderr) ),
                       "Fail to read output streams of executed command."
                       )
                   );

        int exitcode = libssh2_channel_get_exit_status( ch.get() );
        //boost::shared_ptr<LIBSSH2_CHANNEL> ch2 = open_LIBSSH2_CHANNEL();
        return exitcode;
        //return 0;
    }

    int download( const std::string& remote_file_path,
                  const std::string& local_file_path)
    {

        std::ofstream out(local_file_path.c_str());
        struct stat fileinfo;
        boost::shared_ptr<LIBSSH2_CHANNEL> ch(
            libssh2_scp_recv(ss.get(),
                             remote_file_path.c_str(),
                             &fileinfo),
            & SshComm::clean_LIBSSH2_CHANNEL );
        if(!ch)
        {
            //TODO:
            throw_error("fail to open channel.");
        }
        libssh2_channel_set_blocking(ch.get(),0);

        //int res_out;
//        for(;;)
//        {
//            int res_out=read_channel_stream( ch.get(), out, 0 );
//            if(res_out==LIBSSH2_ERROR_EAGAIN)
//                waitsocket();
//            else
//                break;
//        }
        loopAsync( READ,
                   libssh_code_to_async_(
                       boost::bind( & me::read_channel_stream,
                                    this, ch.get(),
                                    boost::ref(out),
                                    0 ),
                       "Fail to read output streams of download.")
                   );


        return 0;
    }

    int upload( const std::string& local_file_path,
                const std::string& remote_file_path)
    {
        struct stat fileinfo;
        stat(local_file_path.c_str(), &fileinfo);

        boost::shared_ptr<LIBSSH2_CHANNEL> ch(
            libssh2_scp_send64( ss.get(),
                                remote_file_path.c_str(),
                                fileinfo.st_mode & 0777,
                                (unsigned long long)fileinfo.st_size,
                                0,
                                0),
            & SshComm::clean_LIBSSH2_CHANNEL );
        //std::cout<< "Open LIBSSH2 channel (upload): " << ch.get() <<"  (session:) " << ss.get() <<std::endl;

        if(!ch)
        {
            throw_error("fail to open channel.");
        }

        libssh2_channel_set_blocking(ch.get(),1);

        std::ifstream inp(local_file_path.c_str(),
                          std::ifstream::binary);
        if(!inp)
        {
            throw_error("fail to open file.");
        }

        std::istreambuf_iterator<char> begin(inp), end;
        std::string data( begin, end );
        int rc;
        //std::cout<<"point 1, data.size: "<< data.size() <<std::endl;
        for( std::string::size_type send_size=0;
                send_size <data.size();
                send_size += buf_size  )
        {
            rc = libssh2_channel_write(ch.get(),
                                       data.c_str() + send_size,
                                       std::min( data.size()-send_size, (std::string::size_type)buf_size ) );
            //std::cout<<"point 2"<<std::endl;
            if (rc < 0) throw_error("fail to write data.");
        }

        rc = libssh2_channel_send_eof(ch.get());
        if (rc < 0) throw_error("fail to send EOF.");
        libssh2_channel_wait_eof(ch.get());
        //std::cout<<"point 3"<<std::endl;

        return 0;
    }

private:
    /// @brief The io_service needed for boost::asio.
    io_service io;
    /// @brief The underlying socket used for ssh communication.
    ip::tcp::socket sck;
    /// @brief The SSH session structure to use in all
    /// communcations using this instance.
    boost::shared_ptr<LIBSSH2_SESSION> ss;
};



std::vector<ip::tcp::endpoint>
hostname_to_endpoints(const gmdString& host, int port=22)
{
    //TODO: remove debug messages
    //std::cout<< " hostname_to_endpoints : "<< host << std::endl;
    io_service io_service_;
    ip::tcp::resolver::query query(host, boost::lexical_cast<std::string>(port) );
    ip::tcp::resolver resolver( io_service_ );

    ip::tcp::resolver::iterator begin= resolver.resolve(query), end;
    //return std::vector<ip::tcp::endpoint>(begin, end);

    std::vector<ip::tcp::endpoint> ret;
    for( ; begin!=end; ++begin)
    {
        //std::cout << begin->host_name() << std::endl;
        //ret.push_back(ip::tcp::endpoint(begin->address(), port ));
        ret.push_back(*begin);
    }

    //std::cout<< " hostname_to_endpoints ret.size: "<< ret.size() <<std::endl;
    //for(size_t i=0; i<ret.size(); ++i)
    //    std::cout<< " ret[" << i<<"] : " << ret[i].address() << ":" << ret[i].port() << std::endl;


    return ret;

}

ip::tcp::endpoint hostname_to_endpoint(const gmdString& host, int port=22)
{
    std::vector<ip::tcp::endpoint> tmp = hostname_to_endpoints(host, port);
    if(tmp.empty())
    {
        //TODO: retport error
        std::cerr<<" Fail to resolve hostname..."<<std::endl;

        return ip::tcp::endpoint( ip::address::from_string(host), port);
        //return ip::tcp::endpoint();
    }

    static int i=0;

    return tmp[ (i++) % tmp.size() ];
}


} // namespace




class gmShellLibssh2::Impl
{
private:
    boost::shared_ptr<void> using_libssh2_;
public:
    // Input
    const gmdString host, login, pw, key_path;
    //const gmdString userhost;
    ip::tcp::endpoint endpoint;
    SshComm sshComm;

    Impl(const gmdString& host, const gmdString& login, const gmdString& pw, const gmdString& key_path)
        : using_libssh2_( using_libssh2::instance() )
        , host(host), login(login), pw(pw), key_path(key_path)
        //, userhost( login +"@" + host )
        , endpoint( hostname_to_endpoint(host, 22) )
        , sshComm( endpoint, login, key_path, pw )
    {}

    Impl(const gmdString& host, const gmdString& login, const gmdString& pw)
        : using_libssh2_( using_libssh2::instance() )
        , host(host), login(login), pw(pw), key_path()
        //, userhost( login +"@" + host )
        , endpoint( hostname_to_endpoint(host, 22) )
        , sshComm( endpoint, login, pw )
    {}
};




gmShellLibssh2::gmShellLibssh2(pCSTR login, pCSTR host) :
    auth_changed(false)
{
    // Global parameters which could be read from XML
    InitParam("login", login);
    InitParam("host", host);
    InitParam("pw", "");
    InitParam("key_path", "");

//  InitParam("plink_args", plink_args);
//  InitParam("plink_path", "_default_");
//  InitParam("pscp_path", "_default_");
//  InitParam("plink_att_num", "1");
//  InitParam("plink_retry_delay", "3000");
}


void gmShellLibssh2::set_param(const gmdString& par_name, const gmdString& par_value)
{
    //bool plink_cmd_changed = false, pscp_cmd_changed = false, auth_changed = false;

    if(par_name == "login" || par_name == "host" || par_name == "pw" || par_name == "key_path" )
    {
        auth_changed = true;
    }
//  else if(par_name == "plink_att_num")
//    plink_att_num = ParseIntParam(par_name, par_value, 1);
//  else if(par_name == "plink_retry_delay")
//    plink_retry_delay =  ParseIntParam(par_name, par_value, 0);
//  else if(par_name == "plink_args") {
//    plink_args = par_value;
//    auth_changed = true;
//  }


    // Check authorization parameters
    if(auth_changed)
    {
        // TODO:
//    auth_defined = !param["host"].IsEmpty() && !param["login"].IsEmpty() &&
//      ( plink_args.Contains("-load") ||
//        plink_args.Contains("-pw") ||
//        plink_args.Contains("-i") );
//      auth_changed=false;
    }

    // TODO:
//  if(auth_defined) {
//    // Create command line templates
//    userhost = param["login"] + "@" + param["host"];
//    if(plink_cmd_changed || auth_changed)
//      plink_pre = "\"" + gmdString(plink_path) + "\" -batch -ssh " + plink_args + " "
//        + (plink_args.Contains("-load") ? "" : userhost) + " \"";
//    if(pscp_cmd_changed || auth_changed)
//      pscp_pre  = "\"" + gmdString(pscp_path) + "\"  -batch -r -q " + plink_args + " \"";
//  }

    gmShell::set_param(par_name, par_value);
}


gmShellLibssh2::Impl& gmShellLibssh2::impl()
{
    if(!pImpl || auth_changed)
    {
        if(dump_commands)
            LOGJOBMSG4( fmt("------ open connection (libssh2) ------\n"
                            " host=%s\n"
                            " login=%s\n"
                            " pw(masked)=%s\n"
                            " key_path=%s",
                            this->param["host"      ].c_str()  ,
                            this->param["login"     ].c_str()  ,
                            this->param["pw"].empty() ? "'' (not set)" : "******"  ,
                            this->param["key_path"  ].c_str()   ));

        if( this->param["key_path"].empty() )
            pImpl = boost::make_shared<Impl>(
                        this->param["host"]     ,
                        this->param["login"]    ,
                        this->param["pw"]       );
        else
            pImpl = boost::make_shared<Impl>(
                        this->param["host"]     ,
                        this->param["login"]    ,
                        this->param["pw"]       ,
                        this->param["key_path"] );
        auth_changed=false;
    }

    return *pImpl;
}

static void string_to_line_array(const gmdString& str, gmdArrayString& arr)
{
    arr.Clear();
    std::string::size_type i = 0, len = str.Len();

    while(i < len)
    {
        int j = str.find_first_of("\r\n", i);
        if(j == std::string::npos) j = len;
        arr.Add( str.Mid(i, j - i) );
        i = j + 1;
        if(i < len && str[j] == '\r' && str[i] == '\n') i++;
    }
}



int gmShellLibssh2::execute(const gmdString& cmd, gmdArrayString& out, gmdArrayString& err)
{
    // Execute Shell command
    int i, res;
    gmdString errmsg;

    execute_begin(cmd, out, err);

    //TODO:
//  if(!auth_defined) {
//    LOGJOBERR(20, "Authentification method is not defined, "
//      "check 'login', 'host' and 'plink_args' parameters!");
//    return -1;
//  }

    // Screed double quotes
    //gmdString cmd_scr(cmd);
    //cmd_scr.Replace("\"", "\\\"");
    // Avoid empty command that switches plink into the interactive mode
    //if( cmd_scr.IsEmpty() ) cmd_scr = "#";

    //gmdString str_out, str_err;
    std::ostringstream stream_out, stream_err;
    ttimer.Resume();
    res = impl().sshComm.execute( cmd.c_str(), stream_out, stream_err );
    ttimer.Pause();
    string_to_line_array(stream_out.str(),out);
    string_to_line_array(stream_err.str(),err);

//  for(i=0; i<plink_att_num; i++){
//    if(i) {  // Retrying on a network error
//      LOGJOBMSG( (errmsg + "\nStill trying ...").c_str() );
//      gmdMilliSleep(plink_retry_delay);  // delay before a retry
//      LOGJOBMSG( fmt("                attempt %d of %d", i+1, plink_att_num) );
//    }

    //TODO:
    //res = gmdExecute(plink_pre + cmd_scr + '\"', out, err);

//    if(res == -1) LOGJOBERR(20, "Error executing PLINK!"); // Fatal error

//    if(!res && !err.IsEmpty())
//      if( err[0].Contains("FATAL ERROR:") ) break; // exit if not a network error

//    if(res==0) break; // normal preform network operation.
//  }

    execute_end(res, out, err);

    return res;
}


//gmdString tempdir, src, err;

//assert_no_recursive(flags);
//assert_file_exists(locfile);

//// Convert text file
//if(flags & TEXT){
//  tempdir = GetTempDir(); // we cannot check if 'rempath' is dir or file name
//  gmdFileName filename(locfile);
//  src = tempdir + "\\" + filename.GetFullName();
//  if( dos2unix(locfile, src) > 0 ) {
//    _remove_local(tempdir);
//    return 1;
//  }
//}
//else
//  src = locfile;

//// Copy file
//gmdString args = "\"" + src + "\" \""
//  + userhost + ':' + rem_path_subst(rempath) + "\"";
//int res = pscp_execute(args, err);
//if( !err.IsEmpty() ) LOGJOBWARN((pCSTR)err.c_str());

//if( !tempdir.IsEmpty() ) _remove_local(tempdir);

//if(!res && (flags & MOVE)) res = RemoveLocal(locfile, false);

//return res;

int gmShellLibssh2::StageIn(pCSTR locfile, pCSTR rempath, unsigned flags)
{

    gmdString tempdir, src;

    assert_no_recursive(flags);
    assert_file_exists(locfile);

    // Convert text file
    if(flags & TEXT)
    {
        tempdir = GetTempDir(); // we cannot check if 'rempath' is dir or file name
        gmdFileName filename(locfile);
        src = tempdir + "\\" + filename.GetFullName();
        if( dos2unix(locfile, src) > 0 )
        {
            _remove_local(tempdir);
            return 1;
        }
    }
    else
        src = locfile;


    if(dump_commands)
        LOGJOBMSG4( fmt("------ upload (libssh2) ------\n"
                        " locfile=%s\n"
                        " rempath=%s\n" ,
                        (pCSTR)locfile  ,
                        (pCSTR)rempath ));
    int res=impl().sshComm.upload( locfile, rempath );
    if( !tempdir.IsEmpty() ) _remove_local(tempdir);
    if( res==0 && (flags & MOVE)) res = RemoveLocal(locfile, false);

    return res;
}


int gmShellLibssh2::StageIn(const gmdArrayString& locfiles, pCSTR remdir, unsigned flags)
{
    gmdString tempdir, src, err;
    gmdArrayString srclist;

    for(unsigned i=0; i<locfiles.GetCount(); i++)
    {
        gmdString lfile = locfiles[i];
        if( lfile.IsEmpty() ) continue;

        if( has_wildcards(lfile) )
        {
            // List all files explicitly if lfile contains wildcards
            gmdArrayString filelist;
            files_by_mask(lfile, filelist, flags);
            for(unsigned k=0; k<filelist.GetCount(); k++) srclist.Add(filelist[k]);
        }
        else
            srclist.Add(lfile);
    }

    if(srclist.IsEmpty()) return 0;

    if(flags & TEXT)
    {
        // Convert all files in the list
        assert_no_recursive(flags);
        tempdir = GetTempDir();
        dos2unix(srclist, tempdir);
        //src = tempdir + "\\*.*";
    }
    else
    {
        //for(unsigned i=0; i<srclist.GetCount(); i++)
        //  src += (i>0 ? "\" \"" : "") + srclist[i];
    }

    // Copy files
    //gmdString args = (flags & RECURSIVE) ? "-r \"" : "\"";
    //args += src + "\" \"" + userhost + ':' + rem_path_subst(remdir) + "\"";
    //int res = pscp_execute(args, err);
    int res=0;
    for(size_t i=0; i<srclist.size(); ++i)
    {
        gmdString src=srclist[i];
        gmdString dst=gmdString(remdir) + "\\" + src ;
        res = impl().sshComm.upload( src, dst );
        if(res!=0)
        {
            LOGJOBWARN("Error at uploading file " + src);
            if( !tempdir.IsEmpty() ) _remove_local(tempdir);
            return res;
        }
    }

    if( !tempdir.IsEmpty() ) _remove_local(tempdir);

    if(!res && (flags & MOVE))
        for(unsigned i=0; i<srclist.GetCount(); i++)
            if( RemoveLocal(srclist[i], flags) ) res = 1;

    return res;
}



int gmShellLibssh2::stage_out(pCSTR locpath, pCSTR rempath, unsigned flags,
                              bool locpath_is_file)
{
    gmdString dst, tempdir;

    if(locpath_is_file) assert_no_recursive(flags);
    if( has_wildcards(rempath) && !locpath_is_file )
    {
        LOGJOBERR("StageIn/StageOut: Can't copy group of files to one file.");
        return -1;
    }

    if(flags & TEXT)
    {
        assert_no_recursive(flags);
        tempdir = GetTempDir();
        dst = tempdir;
    }
    else
        dst = locpath;

    //gmdString rpath( rem_path_subst(rempath) );
    //gmdString args = (flags & RECURSIVE) ? "-r " : "";
    //args += "\"" + userhost + ':' + rpath + "\" \"" + dst + "\"";
    //int res =0;//= pscp_execute(args, err);
    int res =0;
    if( has_wildcards(rempath) )
    {
        res =impl().sshComm.download( rempath,
                                      locpath_is_file ? dst : dst + "\\" + rempath
                                    );
    }
    else
    {
        res =impl().sshComm.download( rempath,
                                      locpath_is_file ? dst : dst + "\\" + rempath
                                    );
    }


    if(flags & TEXT)    // Convert text files
    {
        if(!res)
        {
            gmdArrayString filelist;
            files_by_mask(tempdir + "\\*.*", filelist);

            if(locpath_is_file)
            {
                if(filelist.GetCount() != 1)
                    LOGJOBERR(fmt("StageOut: remote path %s is not a regular file", rempath));

                res = unix2dos(filelist[0], locpath);
                if(res)
                    LOGJOBWARN( fmt("%s => %s: No such file or conversion failed\n",
                                    filelist[0].c_str(), locpath) );
            }
            else
                res = unix2dos(filelist, locpath);
        }

        _remove_local(tempdir);  // Remove temporary file
    }

    //if( !err.IsEmpty() ) LOGJOBWARN((pCSTR)err.c_str());

    if(!res && (flags & MOVE)) res = Remove(rempath);

    return res;
}


int gmShellLibssh2::StageOut(pCSTR locpath, pCSTR remfile, unsigned flags)
{
    return stage_out(locpath, remfile, flags, !gmdDirExists(locpath));
}


int gmShellLibssh2::StageOut(pCSTR locdir, const gmdArrayString& remfiles, unsigned flags)
{
    gmdString dellist, rfile, src;
    int res = 0;

    if( !gmdDirExists(locdir) )
        LOGJOBERR(fmt("Output directory %s does not exists\n",locdir));

    // Copy files one by one as
    // pscp doesn't support more than one remote file
    for(unsigned i=0; i<remfiles.GetCount(); i++)
    {
        int res1 = stage_out(locdir, remfiles[i], flags & ~MOVE, false);
        if((flags & MOVE) && !res1) dellist += remfiles[i] + ' ';
        res = res || res1;
    }

    if( !dellist.IsEmpty() ) res = res || Remove(dellist);  // Remove source files
    return res;
}
/*
int gmShellLibssh2::StageOut(pCSTR locpath, pCSTR rempath, int type){
  gmdString lfile(locpath), rfile(rempath), dstfile, tfile, err;
  int res1 = 0, res2;

  if( rfile.StartsWith("text:", &rfile) ) {
    dstfile = lfile;
    lfile = GetTempFile();
  }

  //res2 = pscp_execute(impl().userhost + ':' + rfile, lfile, err);  // Copy original file
  res2 = impl().sshComm.download( rfile, lfile );

  if( res2==0 && !dstfile.IsEmpty() ) {  // Convert text file
    if( gmdDirExists(dstfile) ) {  // output path is a directory
      gmdFileName filename(rfile, gmdPATH_UNIX);
      if(dstfile.Last() != '\\' && dstfile.Last() != '/') dstfile += '\\';
      dstfile += filename.GetFullName();  // append file name
    }
    res1 = unix2dos(lfile, dstfile);  // convert text file
  }

  if( !dstfile.IsEmpty() && gmdFileExists(lfile) ) _remove_local(lfile);  // Remove temporary file

  if( res1 )
    LOGJOBWARN(fmt("unix2dos: %s: No such file or conversion failed\n", (pCSTR)dstfile.c_str()) );
  if( !err.IsEmpty() ) LOGJOBWARN((pCSTR)err.c_str());

  res1 = res1 || res2;
  if(!res1 && type == fMove) res1 = Remove(rfile);

  return res1;
}


int gmShellLibssh2::StageOut(pCSTR locdir, const gmdArrayString& remfiles, int type){
  gmdString dellist, rfile;
  int res = 0, res1;

  if( !gmdDirExists(locdir) )
    LOGJOBERR(fmt("Output directory %s does not exists\n",locdir));

  // Slow implementation: files are copied one by one!
  for(unsigned i=0; i<remfiles.GetCount(); i++) {
    res1 = StageOut(locdir, remfiles[i], fCopy);
    res = res || res1;

    if(!res1 && type == fMove) {
      if( remfiles[i].StartsWith("text:", &rfile) ) dellist += rfile + ' ';
      else dellist += remfiles[i] + ' ';
    }
  }

  if( !dellist.IsEmpty() ) res = res || Remove(dellist);  // Remove source files
  return res;
}


int gmShellLibssh2::dos2unix(pCSTR src, pCSTR dst){
  FILE *fi = fopen(src, "rb");
  if(!fi) return 1;

  FILE *fo = fopen(dst, "wb");
  if(!fo) {
    fclose(fi);
    return 2;
  }

  int res = 0;
  char c;
  while( (c = fgetc(fi)) != EOF )
    if(c != '\r' && fputc(c, fo) == EOF) { res = 3; break; }  // put any char but '\r'

  fclose(fo);
  fclose(fi);
  return res;
}


int gmShellLibssh2::unix2dos(pCSTR src, pCSTR dst){
  FILE *fi = fopen(src, "rb");
  if(!fi) return 1;

  FILE *fo = fopen(dst, "wb");
  if(!fo) {
    fclose(fi);
    return 2;
  }

  int res = 0;
  char c;
  while( (c = fgetc(fi)) != EOF ) {
    if(c == '\r') continue;  // skip all '\r' in the input file
    if(c == '\n' && fputc('\r', fo) == EOF) { res = 3; break; }  // put '\r' before each '\n'
    if(fputc(c, fo) == EOF) { res = 3; break; }  // put all other chars
  }

  fclose(fo);
  fclose(fi);
  return res;
}
*/
