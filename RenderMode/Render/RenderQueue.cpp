#include "RenderQueue.h"

bool RenderQueue::submit(Command * cmd) {
    if (!empty && head == tail) {
        return false;
    }
    queue[tail] = cmd;
    tail = tail + 1 % queueSize;
    empty = false;

    return true;
}

Command * RenderQueue::pop() {
    if (empty || head == tail) {
        return nullptr;
    }

    Command * cmd = queue[head];
    head = head+ 1 % queueSize;
    if (head == tail) {
        empty = true;
    }
    return cmd;
}

bool RenderQueue::isEmpty() {
    return empty;
}
