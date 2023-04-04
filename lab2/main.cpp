#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <set>

#define REQ 1
#define REL 2
#define OK  3

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int rank;
int processes_total;
int time_label = 0;
int max_time_label = 0;

struct ProcessInfo {
    int rank;
    int time_label;
    int accepted;

    bool operator < (const ProcessInfo &other) const {
        if (time_label == other.time_label) {
            return rank < other.rank;
        }
        return time_label < other.time_label;
    }
};

std::vector<ProcessInfo> processes_info;
std::set<ProcessInfo> messages_queue;

void *receiver(void *args) {

    ProcessInfo *process_info = &processes_info[rank];
    ProcessInfo *recv_process_info;
    MPI_Status recv_status;
    int recv_time_label;
    int recv_rank;
    int recv_tag;
    
    while (true) {

        MPI_Recv(
            &recv_time_label,
            1,
            MPI_INT,
            MPI_ANY_SOURCE,
            MPI_ANY_TAG,
            MPI_COMM_WORLD,
            &recv_status
        );
        
        recv_rank = recv_status.MPI_SOURCE;
        recv_tag = recv_status.MPI_TAG;

        max_time_label = time_label > recv_time_label ? time_label : recv_time_label;

        if (recv_tag == REQ) {
            recv_process_info = &processes_info[recv_rank];
            pthread_mutex_lock(&mutex);
            recv_process_info->time_label = recv_time_label;
            recv_process_info->accepted = 0;
            messages_queue.insert(*recv_process_info);
            pthread_mutex_unlock(&mutex);
            int curr_time_label = time_label;
            MPI_Send(&curr_time_label, 1, MPI_INT, recv_rank, OK, MPI_COMM_WORLD);
            continue;
        } 
        if (recv_tag == REL) {
            recv_process_info = &processes_info[recv_rank];
            pthread_mutex_lock(&mutex);
            messages_queue.erase(*recv_process_info);
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
            continue;
        }
        if (recv_tag == OK) {
            pthread_mutex_lock(&mutex);
            process_info->accepted++;
            pthread_mutex_unlock(&mutex);
            if (process_info->accepted == processes_total - 1) {
                pthread_cond_signal(&cond);
            }
            continue;
        }
    }
}

void send_message(int message, int tag) {
    for (int i = 0; i < processes_total; ++i) {
        if (i != rank) {
            MPI_Send(&message, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
        }
    }
}

bool is_current_process_first_in_messages_queue() {
    ProcessInfo *process_info = &processes_info[rank];
    auto first_process_info = messages_queue.begin();
    return (
        process_info->rank == first_process_info->rank &&
        process_info->time_label == first_process_info->time_label &&
        process_info->accepted == processes_total - 1
    );
}

void *sender(void *args) {

    ProcessInfo *process_info = &processes_info[rank];
    int curr_time_label;

    while (true) {

        pthread_mutex_lock(&mutex);
        time_label++;
        process_info->time_label = time_label;
        process_info->accepted = 0;
        messages_queue.insert(*process_info);

        curr_time_label = time_label;
        send_message(curr_time_label, REQ);

        process_info->accepted = 0;

        while (!is_current_process_first_in_messages_queue()) {
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock(&mutex);

        printf("Message from thread %d\n", rank);
        usleep(10000);

        curr_time_label = time_label;

        pthread_mutex_lock(&mutex);
        messages_queue.erase(*process_info);
        time_label = time_label > max_time_label + 1 ? time_label : max_time_label + 1;
        pthread_mutex_unlock(&mutex);

        send_message(curr_time_label, REL);
    }
}

int main() {

    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &processes_total);

    for (int i = 0; i < processes_total; ++i) {
        ProcessInfo process_info = {i, 0, 0};
        processes_info.push_back(process_info);
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receiver, NULL);

    pthread_t send_thread;
    pthread_create(&send_thread, NULL, sender, NULL);

    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    MPI_Finalize();

    return 0;
}
