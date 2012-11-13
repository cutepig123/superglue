#ifndef __TEST_TASKQUEUELIST_HPP_
#define __TEST_TASKQUEUELIST_HPP_

#include <string>

class TestTaskQueueList : public TestCase {
    struct OpDefault : public DefaultOptions<OpDefault> {
        typedef Enable ListQueue;
    };

    struct Task : public TaskBase<OpDefault> {
        std::string name;
        Task(std::string name_) : name(name_) {}
        void run() {}
    };

    struct IsBPred {
        static bool test(TaskBase<OpDefault> *task) {
            return ((Task *) task)->name == "B";
        }
    };

    static bool testTaskQueue(std::string &name) { name = "testTaskQueue";
        TaskQueue<OpDefault> q;

        if (q.gotWork()) return false;
        q.push_back(new Task("A"));
        if (!q.gotWork()) return false;
        q.push_front(new Task("B"));
        q.push_back(new Task("C"));
        q.push_back(new Task("D"));

        union {
          TaskBase<OpDefault> *taskBase;
          Task *task;
        };

        if (!q.get(taskBase) || task->name != "B") return false;
        if (!q.get(taskBase) || task->name != "A") return false;
        if (!q.get(taskBase) || task->name != "C") return false;
        if (!q.get(taskBase) || task->name != "D") return false;
        if (q.gotWork() || q.get(taskBase)) return false;

        q.push_front(new Task("A"));
        q.push_front(new Task("B"));
        q.push_front(new Task("C"));
        q.push_front(new Task("D"));

        if (!q.steal(taskBase) || task->name != "A") return false;
        if (!q.steal(taskBase) || task->name != "B") return false;
        if (!q.steal(taskBase) || task->name != "C") return false;
        if (!q.steal(taskBase) || task->name != "D") return false;
        if (q.gotWork() || q.steal(taskBase)) return false;

        return true;
    }

    static bool testEraseIf(std::string &name) { name = "testEraseIf";
        union {
          TaskBase<OpDefault> *taskBase;
          Task *task;
        };
        {
            TaskQueue<OpDefault> q;
            q.push_back(new Task("A"));
            q.push_back(new Task("E"));
            q.push_back(new Task("C"));
            q.push_back(new Task("D"));
            q.erase_if(IsBPred());
            if (!q.get(taskBase) || task->name != "A") return false;
            if (!q.get(taskBase) || task->name != "E") return false;
            if (!q.get(taskBase) || task->name != "C") return false;
            if (!q.get(taskBase) || task->name != "D") return false;
            if (q.gotWork() || q.get(taskBase)) return false;
        }
        {
            TaskQueue<OpDefault> q;
            q.push_back(new Task("A"));
            q.push_back(new Task("B"));
            q.push_back(new Task("C"));
            q.erase_if(IsBPred());
            if (!q.get(taskBase) || task->name != "A") return false;
            if (!q.get(taskBase) || task->name != "C") return false;
            if (q.gotWork() || q.get(taskBase)) return false;
        }
        {
            TaskQueue<OpDefault> q;
            q.push_back(new Task("B"));
            q.push_back(new Task("B"));
            q.push_back(new Task("B"));
            q.push_back(new Task("B"));
            q.push_back(new Task("B"));
            q.erase_if(IsBPred());
            if (q.gotWork() || q.get(taskBase)) return false;
        }
        {
            TaskQueue<OpDefault> q;
            q.push_back(new Task("B"));
            q.push_back(new Task("A"));
            q.push_back(new Task("B"));
            q.push_back(new Task("C"));
            q.push_back(new Task("B"));
            q.erase_if(IsBPred());
            if (!q.get(taskBase) || task->name != "A") return false;
            if (!q.get(taskBase) || task->name != "C") return false;
            if (q.gotWork() || q.get(taskBase)) return false;
        }
        {
            TaskQueue<OpDefault> q;
            q.push_back(new Task("A"));
            q.push_back(new Task("B"));
            q.push_back(new Task("B"));
            q.push_back(new Task("C"));
            q.erase_if(IsBPred());
            if (!q.get(taskBase) || task->name != "A") return false;
            if (!q.get(taskBase) || task->name != "C") return false;
            if (q.gotWork() || q.get(taskBase)) return false;
        }

        return true;
    }

public:

    std::string getName() { return "TestTaskQueueList"; }

    testfunction *get(size_t &numTests) {
        static testfunction tests[] = {
            testTaskQueue, testEraseIf
        };
        numTests = sizeof(tests)/sizeof(testfunction);
        return tests;
    }
};

#endif // __TEST_TASKQUEUELIST_HPP_