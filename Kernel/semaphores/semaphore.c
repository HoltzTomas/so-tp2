#include <semaphore.h>
#include <scheduler.h>
#include <memoryManager.h>
#include <lib.h>
#include <stddef.h>

extern int _xchg(int *ptr, int value);

static Semaphore semaphores[MAX_SEMAPHORES];

void semaphore_manager_init(void) {
    memset(semaphores, 0, sizeof(semaphores));
}

static void acquire_lock(Semaphore *sem) {
    while (_xchg(&sem->lock, 1)) {
        uint16_t pid = get_pid();
        Node *node = list_append(&sem->lock_queue, (void *)(uint64_t)pid);
        if (node == NULL)
            continue;
        set_status(pid, BLOCKED);
        yield();
    }
}

static void release_lock(Semaphore *sem) {
    Node *node = list_get_first(&sem->lock_queue);
    if (node != NULL) {
        uint16_t pid = (uint16_t)(uint64_t)list_remove(&sem->lock_queue, node);
        Process *p = get_process_by_pid(pid);
        if (p != NULL && p->status != ZOMBIE)
            set_status(pid, READY);
    }
    sem->lock = 0;
}

int8_t sem_init(uint16_t id, uint32_t initial_value) {
    if (id >= MAX_SEMAPHORES)
        return -1;

    if (semaphores[id].active)
        return -1;

    Semaphore *sem = &semaphores[id];
    sem->value = initial_value;
    sem->lock = 0;
    list_init(&sem->waiting_queue);
    list_init(&sem->lock_queue);
    sem->active = 1;
    return 0;
}

int8_t sem_open(uint16_t id) {
    if (id >= MAX_SEMAPHORES || !semaphores[id].active)
        return -1;
    return 0;
}

int8_t sem_close(uint16_t id) {
    if (id >= MAX_SEMAPHORES || !semaphores[id].active)
        return -1;
    return 0;
}

int8_t sem_destroy(uint16_t id) {
    if (id >= MAX_SEMAPHORES || !semaphores[id].active)
        return -1;

    Semaphore *sem = &semaphores[id];

    while (!list_is_empty(&sem->waiting_queue)) {
        Node *node = list_get_first(&sem->waiting_queue);
        uint16_t pid = (uint16_t)(uint64_t)list_remove(&sem->waiting_queue, node);
        Process *p = get_process_by_pid(pid);
        if (p != NULL && p->status != ZOMBIE)
            set_status(pid, READY);
    }

    while (!list_is_empty(&sem->lock_queue)) {
        Node *node = list_get_first(&sem->lock_queue);
        uint16_t pid = (uint16_t)(uint64_t)list_remove(&sem->lock_queue, node);
        Process *p = get_process_by_pid(pid);
        if (p != NULL && p->status != ZOMBIE)
            set_status(pid, READY);
    }

    sem->active = 0;
    sem->value = 0;
    sem->lock = 0;
    return 0;
}

int8_t sem_wait(uint16_t id) {
    if (id >= MAX_SEMAPHORES || !semaphores[id].active)
        return -1;

    Semaphore *sem = &semaphores[id];

    acquire_lock(sem);

    while (sem->value == 0) {
        uint16_t pid = get_pid();
        Node *node = list_append(&sem->waiting_queue, (void *)(uint64_t)pid);
        if (node == NULL) {
            release_lock(sem);
            return -1;
        }
        set_status(pid, BLOCKED);
        release_lock(sem);
        yield();

        if (!sem->active)
            return -1;

        acquire_lock(sem);
    }

    sem->value--;
    release_lock(sem);
    return 0;
}

int8_t sem_post(uint16_t id) {
    if (id >= MAX_SEMAPHORES || !semaphores[id].active)
        return -1;

    Semaphore *sem = &semaphores[id];

    acquire_lock(sem);
    sem->value++;

    if (!list_is_empty(&sem->waiting_queue)) {
        Node *node = list_get_first(&sem->waiting_queue);
        uint16_t pid = (uint16_t)(uint64_t)list_remove(&sem->waiting_queue, node);
        Process *p = get_process_by_pid(pid);
        if (p != NULL && p->status != ZOMBIE)
            set_status(pid, READY);
    }

    release_lock(sem);
    return 0;
}
