#include <iostream>

#ifdef USING_WXWIDGETS
#include <wx/utils.h>
#endif

#include <gridmd.h>
#include <gmd/threadpool.h>


using namespace gridmd;

#ifdef USING_WXWIDGETS
wxMutex streamMutex;
#endif

void sleep(int seconds) {
#ifdef USING_WXWIDGETS
    wxSleep(seconds);
#endif
}

int gridmd_main(int argc, char* argv[]) {

    sleep(1);
    #ifdef USING_WXWIDGETS
    wxMutexLocker lock(streamMutex);
    #endif

    std::cout << "Task at " << argc << " is executed" << std::endl;
    throw task_finished();
}

int main(int argc,char* argv[]){

    if( !gmdInitialize() ) {
        puts("Failed to initialize the GridMD library.");
        return -1;
    }

    {
        gmdThreadPool pool;
        std::vector<gmTaskID> tasks;

        std::cout << "Fill pool with 50 tasks (1 sec time each)..." << std::endl;
        for (int i = 0; i < 50; ++i) {
            tasks.push_back(pool.CreateGMMainTask(gridmd_main, i, NULL));
        }

        sleep(2);

        for (int i = 0; i < 50; ++i) {
            pool.TaskResult(tasks.at(i));
        }

        std::cout << "Add 14 long time task in pool (120 sec each)" << std::endl;
        std::vector<gmTaskID> longTasks;

        for (int i = 0; i < 14; ++i) {
            longTasks.push_back(pool.CreateScriptTask("sleep 120s"));
        }

        sleep(1);

        for (int i = 0; i < 14; ++i) {
            sleep(1);
            std::cout << "Long task at " << i << " is " << pool.StrTaskStatus(longTasks.at(i)) << std::endl;
        }

        std::cout << "After 5 seconds all of long tasks will be killed.." << std::endl;
        for(int i = 5; i > 0; --i) {
            std::cout << i << "s" << std::endl;
            sleep(1);
        }

        for (int i = 0; i < 14; ++i) {
            pool.RemoveTask(longTasks.at(i));
        }

        std::cout << "Long tasks have been killed! Print Their Status:" << std::endl;
        for (int i = 0; i < 14; ++i) {
            std::cout << "Long task at " << i << " is " << pool.StrTaskStatus(longTasks.at(i)) << std::endl;
        }

//        tasks.clear();
//        std::cout << "Fill pool with 50 tasks (1 sec time each)..." << std::endl;
//        for (int i = 0; i < 50; ++i) {
//            tasks.push_back(pool.CreateGMMainTask(gridmd_main, i, NULL));
//        }

//        sleep(2);
//        for (int i = 0; i < 25; ++i) {
//            pool.TaskResult(tasks.at(i));
//        }

        sleep(3);
        std::cout << "Add new 3 long tasks (120 sec time each)... And kill pool while tasks are processed." << std::endl;
        longTasks.clear();

        for (int i = 0; i < 3; ++i) {
            longTasks.push_back(pool.CreateScriptTask("sleep 120s"));
        }

        for (int i = 0; i < 3; ++i) {
            sleep(1);
            std::cout << "Long task at " << i << " is " << pool.StrTaskStatus(longTasks.at(i)) << std::endl;
        }

        sleep(3);
        std::cout << "Kill thread pool!" << std::endl;
    }
    gmdUninitialize();
    return 0;
}
