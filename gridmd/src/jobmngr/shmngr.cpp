/****************************************************************************
 * \file shmngr.cpp
 * Implementation of SH Job Manager
****************************************************************************/

#include <wx/filename.h>
#include <wx/utils.h>
#include <wx/regex.h>
#include <iostream>

#include "jobmngr/shmngr.h"
#include <boost/preprocessor/stringize.hpp>
#include <boost/regex.hpp>
#include <boost/foreach.hpp>

//#include <boost/format.hpp>
#include "named_formater.h"

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif
namespace
{
    // Internal static variables
    pCSTR script_file   = ".gmSHManager-start.sh";
    pCSTR status_file   = ".gmSHManager-status";
    pCSTR stdout_file   = ".gmSHManager-stdout";
    pCSTR stderr_file   = ".gmSHManager-stderr";
    pCSTR folder_prefix = "shjob-";
    pCSTR cmd_kill = "kill -9 $(sh_data)";
    pCSTR cmd_tasklist="jobs -p";

    /*
        usege of status_function at sh script:
        \verbatim
            # common case
            status_function <process_id>  <status_file>
            # auto extract PID from status file.
            status_function - <status_file>
            # ignore status file information. (Return JOB_FAILED if not run. Ignore JOB_COMPLETED flag.)
            status_function <process_id>
        \endverbatim
    */
    pCSTR status_function
        =   "status_function() { \n"
            "    if [ _$1 = _- ] \n"
            "    then \n"
            "       if [ _`cat $2 | grep JOB_SUBMITTED` = _JOB_SUBMITTED ]" "\n"
            "       then \n"
            "           status_function `cat $2 | head -n 1`  $2 \n" /// cut PID from status file
            "       else \n"
            "           cat $2 | tail -n 1 \n"
            "       fi \n"
            "    else \n"
//            "        if [ _`cat $2 | grep JOB_COMPLETED` = _JOB_COMPLETED ] \n"
//            "        then \n"
//            "            echo JOB_COMPLETED\n"
//            "        else \n"
            "            if ( ps -p $1 > /dev/null 2>&1 ) then \n"
            "              echo JOB_RUNNING\n"
            "            else \n"
            "              if  [ _`cat $2 | grep JOB_COMPLETED` = _JOB_COMPLETED ] \n"
            "              then \n"
            "                  echo JOB_COMPLETED\n"
            "              else \n"
            "                  echo JOB_FAILED\n"
            "              fi;\n"
            "            fi;\n"
//            "        fi;\n"
            "    fi;\n"
            "};\n"
            ;
    pCSTR cut_id_command= " | head -n 1"; // grep specified PID from status file, assume state is SUBMITED (running)

    const wxString tag_end_of_status_file="----------end-of-status-file------------\n";
    const wxString tag_end_of_job_status ="==========end=of=job=staus============\n";
    const wxString tag_job_item_found= "Job found: shjob-";
}
gmSHManager::t_named_arguments gmSHManager::mk_named_arguments()
{
    named_formater::args ret;
    ret
            ("status_file"  , status_file   )
            ("script_file"  , script_file   )
            ("stdout_file"  , stdout_file   )
            ("stderr_file"  , stderr_file   )
            ("folder_prefix", folder_prefix )
            ("rem_tmp_dir"  , shell->rem_tmp_dir.c_str())
            ("sh_dir"       , sh_dir        )
            ("name_prefix"  , name_prefix   )
            ("sh_args"      , sh_args       )
            ("cut_id"       , cut_id_command)
            //("grep_squeue"  , grep_squeue )
            //("squeue_cmd"   , squeue_cmd  )
            ("status_function", status_function)

    ;
    return ret;
}

gmSHManager::t_named_arguments gmSHManager::mk_named_arguments(gmJob &job)
{
    named_formater::args ret=mk_named_arguments();
    ret
            ("job.command"  , job.command )
            //("job.nproc"    , job.nproc    ? job.nproc    : 1 )
            //("job.nthreads" , job.nthreads ? job.nthreads : 1 )
            //("job.walltime" , job.walltime)
            ("jobdir"       , job._dir()  )
            ("id"           , job.GetID() )
    ;
    return ret;
}



namespace {
    pCSTR cmd_resume= "cd $(jobdir); $(shdata)";
    //pCSTR cmd_
}

static const char get_file_id[] = "";


/// Initialize SH Job Manager
/// shell should point to a valid gmShell object
/// service is the optional parameter for start "-q" argument in the form "QueueName@ServerName"
/// username is not used
void gmSHManager::Open(gmShell& shell_, pCSTR service) {
  shell = &shell_;
  sh_args = service ? wxString(service) + " " : "";
}


/// Set additional parameters for start command
void gmSHManager::Configure(pCSTR sh_dir_, pCSTR job_name_prefix) {
  if(sh_dir_) sh_dir = wxString(sh_dir_) + '/';
  if(job_name_prefix) name_prefix = job_name_prefix;

  if(sh_dir.empty())
  {
      wxString err, out;
      int res =shell->Execute( "whereis -b sh", out, err );
      static const wxString prefix="sh: ";
      static const wxString sufix="sh";
      if(res && out.substr(0, prefix.size()) == prefix
             && out.substr(out.size() -  sufix.size()) == sufix )
      {
          sh_dir= out.substr(  prefix.size(),
                                  out.size() - prefix.size() - sufix.size() );
      }
      else
      {
          LOGJOBERR(106, "Configure: Unable to seek 'sh'.");
      }
  }
}


int gmSHManager::Submit(gmJob& job, pCSTR user_id, bool mktemp){
  return _start(job, user_id, mktemp, false);
}


int gmSHManager::Prepare(gmJob& job, pCSTR user_id, bool mktemp){
  return _start(job, user_id, mktemp, true);
}


int gmSHManager::Resubmit(gmJob& job){
  LOGJOBMSG( fmt("Resubmitting job %s", (const char *)job.GetID().c_str()) );
  //return execute_start(job, "cd " + job._dir() + ";" + JobInfo(job)->sh_data);
  return execute_start(job,
                        named_formater(cmd_resume)
                        (mk_named_arguments(job))
                        .str()
                        );
}


// Prepare job, copy input files and submit it provided prepare_only=false
int gmSHManager::_start(gmJob& job, pCSTR user_id, bool mktemp, bool prepare_only){
  wxString id, out, err, jobdir, remote_cmd, rem_copy, sh_cmd;

  t_named_arguments named_arguments=mk_named_arguments(job);
  named_arguments("user_id", user_id);

  // Check command line
  if( job.command.Find('\'') != wxNOT_FOUND ||
      job.command.Find('\"') != wxNOT_FOUND )
    LOGJOBERR(101, "gmJob::command should not contain quotes!");

  // Create job temp dir
  if( mktemp ) { // Generate a unique directory name using 'id' as a prefix
    if( !user_id ) user_id = "void";
    if( shell->Execute(
          named_formater("mkdir -p $(rem_tmp_dir); mktemp -d $(rem_tmp_dir)/$(folder_prefix)$(user_id)-XXXXXXXXXX")
                (named_arguments)
                .str().c_str(),
        jobdir, err )
      ) LOGJOBERR(102, "Unable to create job temporary directory");
    // Extract job id
    size_t prefix_size=named_formater("$(rem_tmp_dir)/$(folder_prefix)")
                (named_arguments)
                .str().size();
    id = jobdir.Mid(prefix_size);
  }
  else { // Use the user-defined id
    jobdir = named_formater("$(rem_tmp_dir)/$(folder_prefix)$(user_id)")
            (named_arguments).str();
    if( shell->MkDir(jobdir) )
      LOGJOBERR(102, "Unable to create job temporary directory");
    id = user_id;
  }

  if( shell->Execute( "cd "+jobdir+"; pwd", jobdir, err )
          || ! err.empty() || jobdir.empty() )
  {
      LOGJOBERR(102, "Unable to 'cd' to job temporary directory");
  };


  // Update job object and save job info
  gmSHJobInfo* jobinfo = new gmSHJobInfo();
  SetJobData(job, id, jobdir, jobinfo);
  jobs.push_back(&job);

  assert( job._dir() == jobdir );
  assert( job._id()  == id     );
  named_arguments( "jobdir", jobdir  )
                 ( "id"    , id      );


  // Stage in input files
  if( StageAllInput(job, rem_copy) )
    LOGJOBERR(103, "Unable to copy local input files");
  if( !rem_copy.IsEmpty() ) remote_cmd += "if " + rem_copy + "; then\n";

  // Go to the temporary directory
  remote_cmd += "cd " + jobdir + '\n';

  // Prepare start script
//  remote_cmd += wxString::Format(
//    "echo '#!/bin/sh\ncd %s\nJOBDIR=`pwd`\n%s\necho JOB_COMPLETED >>$JOBDIR/%s' >%s\n"
//    "chmod u+x %s\n",
//    (const char *)jobdir.c_str(), (const char *)job.command.c_str(), status_file, script_file, script_file);
  std::string script_content=
          named_formater("#!/bin/sh"                   "\n"
                         "cd $(jobdir);"               "\n"
                         "$(job.command)"              "\n"
                         "echo JOB_COMPLETED >>$(jobdir)/$(status_file)")
          (named_arguments).str();

  remote_cmd += named_formater(
              "echo '$(script_content)' >$(jobdir)/$(script_file)" "\n"
              "chmod u+x $(jobdir)/$(script_file)"    "\n"
              )
          (named_arguments)
          ("script_content", script_content)
          .str().c_str();

  sh_cmd = named_formater(
              /*
              "start_job() {\n"
              "  $(sh_dir)sh $(sh_args) $(jobdir)/$(script_file) "
                    ">$(jobdir)/$(stdout_file) 2>$(jobdir)/$(stderr_file)"
                    " & \n"
              "  echo \\$! >$(jobdir)/$(status_file);\n"
              "} \n"
              "if start_job \n"
              "then \n"
              "  cat $(jobdir)/$(status_file);\n"
              "  echo JOB_SUBMITTED >>$(jobdir)/$(status_file);\n"
              "fi;" "\n"
               */
              "  $(sh_dir)sh $(sh_args) $(jobdir)/$(script_file) "
                    ">$(jobdir)/$(stdout_file) 2>$(jobdir)/$(stderr_file)"
                    " & \n"
              "  echo \\$! >$(jobdir)/$(status_file);\n"
              "  cat $(jobdir)/$(status_file);\n"
              "  echo JOB_SUBMITTED >>$(jobdir)/$(status_file);\n"
        )
        (named_arguments)
        .str();


  // Process additional job parameters

  //if(job.walltime)
  // ...  (job.walltime ? wxString::Format(",walltime=%d", job.walltime) : "")
  //TODO: 'job.walltime' is not utilized !!!
    ;

//  start_cmd += named_formater(
//              "  --output=$(stdout_file) --error=$(stderr_file) $(script_file) "
//              " >$(status_file); "
//              "then cat $(status_file);echo JOB_SUBMITTED >>$(status_file);fi" "\n"
//              )
//          (named_arguments).str();

  jobinfo->sh_command = sh_cmd;
  jobinfo->sh_data = sh_cmd;  // Save start command for the delayed run

  remote_cmd += prepare_only ?
    named_formater(  "echo '$(sh_cmd)" "\n"
                     "JOB_PREPARED' >$(jobdir)/$(status_file)")
              (named_arguments)
              ("sh_cmd", (const char *)sh_cmd.c_str())
              .str() :
    sh_cmd;  // Run start after all other commands

  if( !rem_copy.IsEmpty() ) remote_cmd += named_formater(
              "\n"
              "else echo '"                            "\n"
              "JOB_FAILED' >$(jobdir)/$(status_file)"  "\n"
              "fi")
          (named_arguments).str().c_str();

  JobInfo(job)->sh_command=remote_cmd;

  if(prepare_only)
    return ( !shell->Execute(remote_cmd, out, err) && err.IsEmpty() ) ?
      JOB_PREPARED : JOB_FAILED;

  return execute_start(job, remote_cmd);
}


// Execute start command and save SH ID or return to JOB_PREPARED
int gmSHManager::execute_start(gmJob& job, const wxString& cmd){
  wxString out, err;
  int ex_code;

  //static const boost::regex outMatch( "(?:Submitted batch job )(\\d+)" );
  //static const unsigned     outMatchIndex = 1;
  //boost::cmatch matched;
#if DEBUG_LOG
  std::clog << "DBG: execute_start {{{\n"
            << (const char*) cmd.c_str()
            << "\n}}}\n"
            << std::endl;
#endif

  ex_code=shell->Execute(cmd, out, err);

#if DEBUG_LOG
  std::clog << "DBG: start.EXIT_CODE "<< ex_code << std::endl;
  //std::clog << "DBG: start.parsed "<< parsed << std::endl;
  std::clog << "DBG: start.out {{{\n"
            << (const char*) out.c_str()
            << "\n}}}\n"
            << std::endl;
  std::clog << "DBG: start.err {{{\n"
            << (const char*) err.c_str()
            << "\n}}}\n"
            << std::endl;
#endif
  wxString sh_data=interpret_start(out);

  if(        ex_code==0
          && err.IsEmpty()
          && !sh_data.empty()
          && sh_data.find_first_not_of("0123456789") == std::string::npos
          ){ // no errors & valid job id is returned
      JobInfo(job)->sh_data = sh_data ;
      //JobInfo(job)->sh_data = out;
      return JOB_SUBMITTED;
  }

#if DEBUG_LOG
  std::clog << "DBG: start.out {{{\n"
            << (const char*) out.c_str()
            << "\n}}}\n"
            << std::endl;
  std::clog << "DBG: start.err {{{\n"
            << (const char*) err.c_str()
            << "\n}}}\n"
            << std::endl;
#endif

  // Check if the queue is overloaded
  return (
    err.StartsWith("start: Maximum number of jobs already in queue") &&
    !shell->Execute( named_formater(
          "cd $(jobdir)" "\n"
          "echo 'Prepared batch job $(sh_data)"  "\n"
                "JOB_PREPARED' >./$(status_file)"
        )
        ( mk_named_arguments(job) )
        ("sh_data", (const char *)JobInfo(job)->sh_data.c_str())
        .str() )
    ) ? JOB_PREPARED : JOB_FAILED ;
}


/*wxString gmSHManager::Status_cmd(gmJob& job){
  return wxString::Format(
    "if grep End %s/%s; then\necho C\nelse\n"
    "%ssqueue -f %s | grep 'job_state ='\n"
    "if [ $? -ne 0 ]; then echo F; fi\nfi",
    job._dir().c_str(), status_file,
    sh_dir.c_str(), JobInfo(job)->sh_data.c_str() )
}

int gmSHManager::Status_analyze(wxString& out){
}
*/


// Analyze output of the script
int gmSHManager::interpret_status(const wxString& str) {

#if DEBUG_LOG
    std::clog << "DBG: interpret_status {{{\n"
              << (const char*) str.c_str()
              << "\n}}}\n"
              << std::endl;
#endif

  if( str.Find("JOB_COMPLETED"  ) != wxNOT_FOUND ) return JOB_COMPLETED;
  if( str.Find("JOB_FAILED"     ) != wxNOT_FOUND ) return JOB_FAILED;
  if( str.Find("JOB_SUSPENDED"  ) != wxNOT_FOUND ) return JOB_SUSPENDED;
  if( str.Find("JOB_PREPARED"   ) != wxNOT_FOUND ) return JOB_PREPARED;
  if( str.Find("JOB_RUNNING"    ) != wxNOT_FOUND ) return JOB_RUNNING;
  if( str.Find("JOB_SUBMITTED"  ) != wxNOT_FOUND ) return JOB_SUBMITTED;

  {
#if DEBUG_LOG
      std::clog << "DBG:  Not paresed!." << std::endl;
#endif
      throw std::runtime_error("Fail to parse squeue resuilt");
  }
}

/**
interpret 'out' string at fomat:
\verbatim
 Job found: shjob-jobid_1
 text of stauts fle (1)
 ----------end-of-status-file------------
 result of status_function
 ==========end=of=job=staus============
 Job found: shjob-jobid_2
 text of stauts fle (2)
 ----------end-of-status-file------------
 result of status_function (2)
 ==========end=of=job=staus============
 ...
\endverbatim
**/
std::vector<gmSHManager::squeue_result_item> gmSHManager::interpret_status_list(wxString out)
//int gmSHManager::interpret_squeue_list(wxString out, boost::function<t_interpret_squeue_list_callback> callback)
{
#if DEBUG_LOG
    std::clog << "DBG: interpret_status_list {{{\n"
              << (const char*) out.c_str()
              << "\n}}}\n"
              << std::endl;
#endif


    std::vector<squeue_result_item> ret;

    out += "\n";

    for( wxString::size_type i;
         (i=out.find(tag_end_of_job_status))!=wxString::npos;
         out=out.substr(i+tag_end_of_job_status.size()) )
    {
        wxString item=out.substr(0, i);
        squeue_result_item r;

        if( item.substr(0, tag_job_item_found.size()) == tag_job_item_found )
        {
            r.id = item.BeforeFirst('\n').substr(tag_job_item_found.size());
            item=item.AfterFirst('\n');
        }
        else
        {
            //TODO: report error ID
        }

        wxString::size_type n=item.find(tag_end_of_status_file);
        if(n!=wxString::npos)
        {
            r.status_file_content = item.substr(0,n);

            wxString status= r.status_file_content.BeforeLast('\n').AfterLast('\n');
            if( status == "JOB_SUBMITTED" )  // Get the status of submited job
              r.state = interpret_status( item.substr( n+tag_end_of_status_file.size() ) );
            else if( status == "JOB_PREPARED" )  // Job is prepared but not submitted
              r.state = JOB_PREPARED;
            else if( status == "JOB_FAILED" )
              r.state = JOB_FAILED;
            else
              r.state =-1;
        }
        else
        {
            r.status_file_content = item;
            r.state =JOB_FAILED;
            //TODO: report error ... ?
        }

        if( ! r.status_file_content.empty() )
        {
            if(r.state != JOB_PREPARED)
                r.sh_data = interpret_start( r.status_file_content.BeforeFirst('\n') );
        }

        ret.push_back(r);

#if DEBUG_LOG
    std::clog << "DBG: interpret_status_list.item {{{\n"
                    << (const char*) item.c_str()
                    << "\n}}}\n"
              << "DBG: interpret_status_list.r.id {{{" << r.id.c_str() <<"}}}\n"
              << "DBG: interpret_status_list.r.sh_data {{{" << r.sh_data.c_str() <<"}}}\n"
              << "DBG: interpret_status_list.r.status_file_content {{{\n"
                    << r.status_file_content.c_str()
                    <<"\n}}}\n"
              << "DBG: interpret_status_list.r.state {{{" << r.state <<"}}}\n"
              <<std::endl
              ;
#endif

    }

    return ret;
}

/**
    Parse output like 'Submitted batch job 226785'.
    Return job ID (number as string).
    Return empty string, on prse fail.
  **/
wxString gmSHManager::interpret_start(const wxString &str)
{
    return str;

}


int gmSHManager::Status(gmJob& job){
  wxString out, err, cmd;
  int res;
  cmd = named_formater(
              "cd $(jobdir) \n"
              "$(status_function)" "\n"
              "status_function $(sh_data) ./$(status_file) \n"
              )
          ( mk_named_arguments(job) )
          ( "sh_data", (const char *)JobInfo(job)->sh_data.c_str() )
          .str();

#if DEBUG_LOG
    std::clog << "DBG: Status.Job.sh_data {{{\n"
              << (const char *)JobInfo(job)->sh_data.c_str()
              << "\n}}}\n"
              << "DBG: Status.cmd {{{\n"
              << (const char *) cmd.c_str()
              << "\n}}}\n"
              << "DBG: Status.Job.dir {{{ " << job.GetWorkDir() << " }}}\n"
              << "DBG: Status.Job.sh_data {{{ " << (const char *)JobInfo(job)->sh_data.c_str() << " }}}\n"
              //<< std::endl
              ;
#endif

  res = shell->Execute( cmd, out, err );
#if DEBUG_LOG
    std::clog << "DBG: Status.exit_code {{{ " << res <<" }}}\n"
              << "DBG: Status.out {{{\n"
              << (const char *) out.c_str()
              << "\n}}}\n"
              << "DBG: Status.err {{{\n"
              << (const char *) err.c_str()
              << "\n}}}\n"
              << std::endl;
#endif

  return (res || !err.IsEmpty()) ? JOB_FAILED : interpret_status(out);
}


void gmSHManager::Stop(gmJob& job){
  shell->Execute(sh_dir + "scancel " + JobInfo(job)->sh_data);
}


int gmSHManager::Attach(gmJob& job, pCSTR id){
  wxString out, err;
  wxString sh_data, jobdir = shell->rem_tmp_dir + "/" + folder_prefix + id;

  // Run the script the looks for the job with the given id
  int res = shell->Execute(
              named_formater(
                  "$(status_function)" "\n"
                  "if [ -f $(jobdir)/$(status_file) ]; then "             "\n"
                  "  echo Job found: `basename $(jobdir)`"                "\n"
                  "  cat $(jobdir)/$(status_file)"                        "\n"
                  "  echo \"----------end-of-status-file------------\""   "\n"
                  "  status_function - $(jobdir)/$(status_file)"                  "\n"
                  "    echo '==========end=of=job=staus============';\n"
                  "else echo No job; \n"
                  "fi"
                  )
                ("jobdir", jobdir)
                ( mk_named_arguments(job) )
                .str(),
    out, err);

#if DEBUG_LOG
    std::clog << "DBG: Attach.out {{{\n"
              << out.c_str()
              << "\n}}}\n"
              << "DBG: Attach.err {{{\n"
              << err.c_str()
              << "\n}}}\n"
              << "DBG: Attach.res {{{ " << res <<" }}}\n"
              << std::endl
              ;
#endif

  // Interpret script output
  if(out == "No job") return JOB_INIT;   // Job is not found

  std::vector<squeue_result_item> result_items=interpret_status_list(out);
  // assert result_items.size()==1
  if(result_items.size()!=1)
  {
      // TODO: report error
      return JOB_INIT;
  }

  sh_data = result_items[0].sh_data;
  if(sh_data.empty())
  {
      // TODO: report error state.
      return JOB_INIT;
  }

  res=result_items[0].state;
  if(res<0)
  {
      // TODO: report error state. (Fail to parse output.)
      return JOB_INIT;
  }

  // Save job information
  SetJobData(job, id, jobdir, new gmSHJobInfo(sh_data));
  jobs.push_back(&job);

  return res;
}

/****************************************************************************
 * Batch functions for a job list
****************************************************************************/

JobList gmSHManager::RestoreAll(pCSTR id_prefix){
  JobList newjobs;
  wxString out, err, id, sh_data, status;
  if(!id_prefix) id_prefix = "";

  // Run the script the looks for all jobs with the given id_prefix
  shell->Execute( named_formater(
      "$(status_function)"                                      "\n"
      "cd $(rem_tmp_dir)"                                       "\n"
      "for JOB in $(folder_prefix)$(id_prefix)*; do"            "\n"
      "  STATF=\\$JOB/$(status_file)"                           "\n"
      "  if [ -f \\$STATF ]; then"                              "\n"
      "    echo Job found: \\$JOB"                              "\n"
      "    cat \\$STATF"                                        "\n"
      "    echo \"----------end-of-status-file------------\""   "\n"
      "    status_function - \\$STATF "                         "\n"
      "    echo '==========end=of=job=staus============';"      "\n"
      "  fi;"                                                   "\n"
      "done")
      (mk_named_arguments())
      ("id_prefix", id_prefix)
      .str(),
    out, err);

  std::vector<squeue_result_item> result_items=interpret_status_list(out);
  BOOST_FOREACH( squeue_result_item item, result_items )
  {
      if(JobByID(item.id))
      {
          LOGJOBWARN(fmt("RestoreAll: job %s ignored as it already exists in Job Manager", (const char *)id.c_str()));
          continue;
      }
      if(item.status_file_content.empty())
      {
          continue;
      }
      if(item.state<0)
      {
          continue;  // Error of parse result
      }

      gmJob* job = CreateJob();
      SetJobData(*job, item.id, shell->rem_tmp_dir + "/"+folder_prefix + item.id,
        new gmSHJobInfo(item.sh_data), item.state);
      newjobs.push_back(job);
      jobs.push_back(job);
  }

  drain_prepared();

  return newjobs;

}


int gmSHManager::WaitAll(pCSTR id_prefix){
  JobList pjobs, rjobs;
  wxString out, err, list, status;
  wxStopWatch timer;
  int res = JOB_COMPLETED, state;
  int not_first_iter = 0;

  // Create list of the selected jobs
  FOR_EACH_LOOP(JobList, jobs, j)
    if( !id_prefix || (*j)->GetID().StartsWith(id_prefix) ) pjobs.push_back(*j);

  if( pjobs.empty() ) return JOB_INIT;

  while( wait_timeout == 0 || timer.Time() < wait_timeout ){
    // Process the current list
    list.Clear();
    rjobs.clear();
    for(JobList::iterator it=pjobs.begin(); it!=pjobs.end();){
      gmJob* j = *(it++);
      state = j->_state();
      if(state == JOB_FAILED) res = JOB_FAILED;
      if(state >= JOB_COMPLETED) pjobs.remove(j);  // remove all completed jobs
      else if(state >=JOB_SUBMITTED){
        rjobs.push_back(j);  // create list of
        list += ' ' + j->GetID();
      }
    }

    if( pjobs.empty() ) break;  // normal loop exit

    // DEBUG!!!
    //printf("Jobs submitted: %d, Jobs prepared: %d\nWaiting for jobs:", jobs_subm, jobs_wait);
    //  FOR_EACH_LOOP(JobList, jobs, j) printf(" %s (state=%d)", (*j)->GetID().c_str(), (*j)->_state());
    //puts("");

    if(not_first_iter)
      wxMilliSleep(wait_delay), wait_time += wait_delay;  // wait 3 seconds before the next try
    else not_first_iter = true;

    // Run the script to get status of the running jobs
    if( !list.IsEmpty() ) {  // monitor SUBMITTED jobs
      shell->Execute( named_formater(
          "$(status_function)" "\n"
          "cd $(rem_tmp_dir)"                                       "\n"
          "for JOB in $(list); do"                                  "\n"
          "  STATF=$(folder_prefix)\\$JOB/$(status_file)"           "\n"
          "  if [ -f \\$STATF ]; then"                              "\n"
          "    echo Job found: $(folder_prefix)\\$JOB"              "\n"
          "    cat \\$STATF "                                       "\n"
          "    echo \"----------end-of-status-file------------\""   "\n"
          "    status_function - \\$STATF "                         "\n"
          "  fi;"                                                   "\n"
          "  echo '==========end=of=job=staus============';\n"
          "done")
                      (mk_named_arguments())
                      ("list", list)
                      .str()
            ,
        out, err);

      if( out.IsEmpty() ) LOGJOBERR(105, "WaitAll is unable to inquire job states");

      std::vector<squeue_result_item> result_items=this->interpret_status_list(out);
      int item_idx=0;

      FOR_EACH_LOOP(JobList, rjobs, j) {
        // Change state of the completed or failed jobs (invokes state_change and runs waiting jobs)
          assert( (**j).GetID()==result_items[item_idx].id );

        SetJobData( **j, NULL, NULL, NULL,  result_items[item_idx].state );
        ++item_idx;
      }
    }
    else drain_prepared();  // try to resubmit PREPARED jobs
  }

  return res;
}


int gmSHManager::FetchAll(bool fWait, pCSTR id_prefix){
  if(fWait) WaitAll(id_prefix);
  return gmJobManager::FetchAll(false, id_prefix);
}


void gmSHManager::StopAll(pCSTR id_prefix){
  wxString list;

  fBatchOperation = true;

  // Create list of all running jobs
  FOR_EACH_LOOP(JobList, jobs, j)
    if( !id_prefix || (*j)->GetID().StartsWith(id_prefix) ) {
      if((*j)->_state() >= JOB_SUBMITTED && (*j)->_state() < JOB_COMPLETED)
        list += ' ' + JobInfo(**j)->sh_data;
      SetJobData(**j, NULL, NULL, NULL, JOB_FAILED);
    }

  if( !list.IsEmpty() ) shell->Execute(sh_dir + "kill -9 " + list);

  fBatchOperation = false;
  drain_prepared();
}


void gmSHManager::ClearAll(pCSTR id_prefix){
  StopAll(id_prefix);
  gmJobManager::ClearAll(id_prefix);
}


wxString gmSHManager::GetStdOut(gmJob& job){
  return job.GetState() >= JOB_COMPLETED ? stdout_file : "";
}

wxString gmSHManager::GetStdErr(gmJob& job){
    return job.GetState() >= JOB_COMPLETED ? stderr_file : "";
}


