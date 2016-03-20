#include <iostream>
#include <wx/utils.h>
#include "gridmd.h"

using namespace gridmd;

#include <threadpool/gmthreadpool.h>
#include <threadpool/gmmaintask.h>
#include <threadpool/gmscripttask.h>

wxMutex streamMutex;

int gridmd_main(int argc, char* argv[]) {
    wxSleep(1);
    wxMutexLocker lock(streamMutex);
    std::cout << "Task at " << argc << " is executed" << std::endl;
    throw task_finished();
}

int main(int argc,char* argv[]){

    if( !gmdInitialize() ) {
        puts("Failed to initialize the GridMD library.");
        return -1;
    }

    gmThreadPool* pool = new gmThreadPool;
    std::vector<gmTask*> tasks;

    std::cout << "Fill pool with 50 tasks..." << std::endl;
    for (int i = 0; i < 50; ++i) {
        tasks.push_back(new gmMainTask(gridmd_main, i, NULL));
    }

    wxSleep(1);

    for (int i = 0; i < 50; ++i) {
        pool->SubmitTask(tasks.at(i));
    }

    for (int i = 0; i < 50; ++i) {
        tasks.at(i)->Result();
    }

    std::cout << "Add 7 long time task in pool" << std::endl;
    std::vector<gmTask*> longTasks;

    for (int i = 0; i < 7; ++i) {
        longTasks.push_back(new gmScriptTask("sleep 120s"));
    }

    wxSleep(1);

    for (int i = 0; i < 7; ++i) {
        pool->SubmitTask(longTasks.at(i));
    }

    for (int i = 0; i < 7; ++i) {
        wxSleep(1);
        std::cout << "Long task at " << i << " is " << longTasks.at(i)->strStatus() << std::endl;
    }

    std::cout << "After 5 seconds all of long tasks will be killed.." << std::endl;
    for(int i = 5 ;i > 0; --i) {
        std::cout << i << "s" << std::endl;
        wxSleep(1);
    }

    for (int i = 0; i < 7; ++i) {
        longTasks.at(i)->Kill();
    }

    std::cout << "Long tasks have been killed!" << std::endl;

    for (int i = 0; i < 7; ++i) {
        std::cout << "Long task at " << i << " is " << longTasks.at(i)->strStatus() << std::endl;
    }

    wxSleep(3);
    std::cout << "Add new tasks..." << std::endl;

    for (int i = 0; i < 50; ++i) {
        pool->SubmitTask(tasks.at(i));
    }

    wxSleep(2);

    for (int i = 0; i < 50; ++i) {
        tasks.at(i)->Result();
    }

    delete pool;
    gmdUninitialize();
    return 0;
}
