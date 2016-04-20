#include <iostream>

#ifdef USING_WXWIDGETS
#include <wx/utils.h>
#endif

#include <gridmd.h>
#include <gmd/threadpool.h>
#include <gmd/redirector.h>

using namespace gridmd;

#ifdef USING_WXWIDGETS
wxMutex streamMutex;
#endif

void sleep(int seconds) {
#ifdef USING_WXWIDGETS
    wxSleep(seconds);
#endif
}

/** \file redirector_example.cpp
    \en \brief Example that tests and illustates various \ref gmRedirector use cases.

**/

int gridmd_main(int argc, char* argv[]) {

    sleep(1);
    throw task_finished();
}


struct TestType1 {
public :

    TestType1(int a, int b) :
        mA(a),
        mB(b)
    {;}

    int mA;
    int mB;
};

struct TestType2 {
public :

    TestType2(const gmdString& str ) :
        mStr(str)
    {;}

    gmdString mStr;
};

int test_type1(int argc, char* argv[]) {

    sleep(1);
    #ifdef USING_WXWIDGETS
    wxMutexLocker lock(streamMutex);
    #endif

    std::cout << "Task with index = " << argc <<  " is executed" << std::endl;

    gmdRedirectorBase* baseRedirector = gmdThreadPool::GetRedirector(typeid(TestType1));
    if (baseRedirector) {

        gmdRedirector<TestType1>* redirector = static_cast<gmdRedirector<TestType1>* > (baseRedirector);
        TestType1* object = redirector->GetObject();
        if (object) {
            std::cout << "Task " << argc << ": " << "Get object of TestType1. Object Contents: " <<
                         object->mA  << " " << object->mB << "\n" << std::endl;
            object->mA++;
            object->mB++;
        }
    }

    throw task_finished();
}

int test_type1_long(int argc, char* argv[]) {

    sleep(1);
    #ifdef USING_WXWIDGETS
    streamMutex.Lock();
    #endif

    std::cout << "Task with index = " << argc <<  " is executed" << std::endl;

    gmdRedirectorBase* baseRedirector = gmdThreadPool::GetRedirector(typeid(TestType1));
    if (baseRedirector) {

        gmdRedirector<TestType1>* redirector = static_cast<gmdRedirector<TestType1>* > (baseRedirector);
        TestType1* object = redirector->GetObject();
        if (object) {
            std::cout << "Task " << argc << ": " << "Get object of TestType1. Object Contents: " <<
                         object->mA  << " " << object->mB << "\n" << std::endl;
            object->mA++;
            object->mB++;
            #ifdef USING_WXWIDGETS
            streamMutex.Unlock();
            #endif
        }
        sleep(120);
    }

    throw task_finished();
}

int test_type2(int argc, char* argv[]) {

    sleep(1);
    #ifdef USING_WXWIDGETS
    wxMutexLocker lock(streamMutex);
    #endif

    std::cout << "Task with index = " << argc <<  " is executed" << std::endl;

    gmdRedirectorBase* baseRedirector = gmdThreadPool::GetRedirector(typeid(TestType2));
    if (baseRedirector) {

        gmdRedirector<TestType2>* redirector = static_cast<gmdRedirector<TestType2>* > (baseRedirector);
        TestType2* object = redirector->GetObject();
        if (object) {
            std::cout << "Task " << argc << ": " << "Get object of TestType2. Object Contents: " <<
                         object->mStr << std::endl;
            sleep(1);
            std::cout << "REMOVE OBJECT IN THE EXECUTING FUNCTION!" << std::endl;
            redirector->RemoveObject();
            sleep(1);
            std::cout << "Get new copy of object within the same execution..." << std::endl;
            TestType2* newCopy = redirector->GetObject();
            sleep(1);
            std::cout << "New copy contents: " << newCopy->mStr << "\n" << std::endl;
        }
    }

    throw task_finished();
}


int main(int argc, char* argv[]){

    if( !gmdInitialize() ) {
        puts("Failed to initialize the GridMD library.");
        return -1;
    }

    {
        gmdThreadPool pool;
        std::vector<gmTaskID> tasks;

        std::cout << "Register TestType1(5, 10) and TestType2(\"Hello!\") prototypes in Thread pool..." << std::endl;
        TestType1 prototype1(5, 10);
        TestType2 prototype2(gmdString("Hello!"));
        pool.RegisterPrototypedRedirector(prototype1);
        pool.RegisterPrototypedRedirector(prototype2);
        sleep(4);

        std::cout << "Get TestType1 object for the MAIN thread..." << std::endl;
        gmdRedirectorBase* baseRedirector = gmdThreadPool::GetRedirector(typeid(TestType1));
        if (baseRedirector) {

            gmdRedirector<TestType1>* redirectorType1 = static_cast<gmdRedirector<TestType1>* > (baseRedirector);
            TestType1* object = redirectorType1->GetObject();
            if (object) {
                std::cout << "Object contents : " << object->mA  << " " << object->mB << "\n" << std::endl;
                sleep(4);
                std::cout << "Alter TestType1 object for the MAIN thread..." << std::endl;
                object->mA++;
                object->mB++;
                sleep(4);

                TestType1* alteredObject = redirectorType1->GetObject();
                std::cout << "Altered object contents : " << alteredObject->mA  << " " << alteredObject->mB << "\n" << std::endl;
            }
        }

        sleep(4);
        std::cout << "Make 50 tasks to test TestType1 objects (1 sec time each)...\n" << std::endl;
        sleep(4);
        for (int i = 0; i < 50; ++i) {
            tasks.push_back(pool.CreateGMMainTask(test_type1, i, NULL));
        }

        for (int i = 0; i < 50; ++i) {
            pool.TaskResult(tasks.at(i));
        }
        tasks.clear();




        std::cout << "Add 20 long time task in pool (120 sec each) and kill them to "
                     "check how the individual task object is removed during the task "
                     "is being executed.\n" << std::endl;

        sleep(2);
        for (int i = 0; i < 20; ++i) {
            tasks.push_back(pool.CreateGMMainTask(test_type1_long, i, NULL));
        }

        sleep(5);
        #ifdef USING_WXWIDGETS
        streamMutex.Lock();
        #endif
        std::cout << "After 5 seconds all of long tasks will be killed.." << std::endl;
        for(int i = 5; i > 0; --i) {
            std::cout << i << "s" << std::endl;
            sleep(1);
        }

        for (int i = 0; i < 20; ++i) {
            pool.RemoveTask(tasks.at(i));
        }

        std::cout << "Tasks killed!" << std::endl;
        #ifdef USING_WXWIDGETS
        streamMutex.Unlock();
        #endif
        tasks.clear();




        std::cout << "Now we will make 50 long time tasks to test TestType2 objects "
                     "with the deletion of object during the function execution. "
                     "Wait only for 5 first tasks finished, then delete thread pool. \n" << std::endl;

        sleep(5);
        for (int i = 0; i < 50; ++i) {
            tasks.push_back(pool.CreateGMMainTask(test_type2, i, NULL));
        }

        sleep(5);

        for (int i = 0; i < 5; ++i) {
            pool.TaskResult(tasks.at(i));
        }
        sleep(2);
    }
    gmdUninitialize();
    return 0;
}
