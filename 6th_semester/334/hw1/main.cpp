#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <vector>
#include "parser.h"

#define HW1_BUFFER_SIZE 4096


typedef std::vector<char**> child_process_t;
typedef std::unordered_map<std::string, child_process_t*> bundle_map_t;


void execute_child_process(char** argv, int* fd, int i, int n)
{
    dup2(fd[2 * i], STDIN_FILENO);
    for (int i = 0; i < n; i++)
    {
        close(fd[2 * i]);
        close(fd[2 * i + 1]);
    }

    execvp(argv[0], argv);
    exit(0);
}


void execute_bundle_process(child_process_t* child_processes, int coming_pipe[2], int going_pipe[2])
{
    int n = child_processes->size();
    pid_t* child_pids = new pid_t[n];
    int* fd = new int[2 * n];

    // stdin <-- read end of coming pipe
    if (coming_pipe[0] != STDIN_FILENO)
    {
        dup2(coming_pipe[0], STDIN_FILENO);
        close(coming_pipe[0]);
    }
    // stdout <-- write end of going pipe
    if (going_pipe[1] != STDOUT_FILENO)
    {
        dup2(going_pipe[1], STDOUT_FILENO);
        close(going_pipe[1]);
    }
    if (coming_pipe[1] > STDERR_FILENO)
        close(coming_pipe[1]);
    if (going_pipe[0] > STDERR_FILENO)
        close(going_pipe[0]);

    // create a pipe for each children
    for (int i = 0; i < n; i++)
        pipe(fd + 2 * i);

    // fork children
    for (int i = 0; i < n; i++)
    {
        child_pids[i] = fork();
        if (child_pids[i] == 0)
            execute_child_process((*child_processes)[i], fd, i, n);
    }

    // close read ends of pipes, these will be read by children
    for (int i = 0; i < n; i++)
        close(fd[2 * i]);

    // close write end of going pipe, this will be written by children
    close(STDOUT_FILENO);

    // repeat input
    char buffer[HW1_BUFFER_SIZE];
    int read_size;
    while ((read_size = read(STDIN_FILENO, buffer, HW1_BUFFER_SIZE - 1)) > 0) {
        buffer[read_size] = '\0';
        for (int i = 0; i < n; i++) {
            write(fd[2 * i + 1], buffer, read_size);
        }
    }

    // close write ends of pipes
    for (int i = 0; i < n; i++)
        close(fd[2 * i + 1]);

    delete[] fd;

    // wait for children
    int child_status;
    for (int i = 0; i < n; i++)
        waitpid(child_pids[i], &child_status, 0);

    delete[] child_pids;
    exit(0);
}


void execute_bundle_command(bundle_map_t& bundle_map, int bundle_count, bundle_execution* bundles)
{
    // int input_fd, output_fd;
    int coming_pipe[2] = {-1, -1};
    int going_pipe[2] = {-1, -1};
    pid_t* bundle_pids = new pid_t[bundle_count];

    // the first coming pipe
    // (going pipe will be copied into coming pipe at the first iteration)
    if (bundles[0].input != nullptr)
        going_pipe[0] = open(bundles[0].input, O_RDONLY);
    else
        going_pipe[0] = STDIN_FILENO;

    // bundle
    for (int i = 0; i < bundle_count; i++)
    {
        // going pipe becomes coming pipe
        coming_pipe[0] = going_pipe[0];
        coming_pipe[1] = going_pipe[1];

        // initialize going pipe
        if (i == bundle_count - 1)
        {
            // the last going pipe
            if (bundles[i].output != nullptr)
            {
                going_pipe[0] = -1;
                going_pipe[1] = open(bundles[i].output, O_WRONLY | O_APPEND | O_CREAT, 0644);
            }
            else
            {
                going_pipe[0] = -1;
                going_pipe[1] = STDOUT_FILENO;
            }
        }
        else
        {
            pipe(going_pipe);
        }

        // fork a bundle repeater process
        bundle_pids[i] = fork();
        if (bundle_pids[i] == 0)
            execute_bundle_process(bundle_map[bundles[i].name], coming_pipe, going_pipe);

        // close previous pipe
        if (coming_pipe[0] > STDERR_FILENO)
            close(coming_pipe[0]);
        if (coming_pipe[1] > STDERR_FILENO)
            close(coming_pipe[1]);
    }

    // close remaining pipes
    if (going_pipe[0] > STDERR_FILENO)
        close(going_pipe[0]);
    if (going_pipe[1] > STDERR_FILENO)
        close(going_pipe[1]);

    // delete allocated memories
    for (int i = 0; i < bundle_count; i++)
    {
        child_process_t* ch_pr = bundle_map[bundles[i].name];
        for (int i = 0; i < ch_pr->size(); i++)
        {
            for (int j = 0; (*ch_pr)[i][j]; j++)
            {
                delete[] (*ch_pr)[i][j];
                (*ch_pr)[i][j] = nullptr;
            }
            delete[] (*ch_pr)[i];
            (*ch_pr)[i] = nullptr;
        }
        delete bundle_map[bundles[i].name];
        bundle_map.erase(bundles[i].name);
    }

    // wait for children (bundle repeaters)
    int bundle_status;
    for (int i = 0; i < bundle_count; i++)
        waitpid(bundle_pids[i], &bundle_status, 0);

    delete[] bundle_pids;
}


void loop(bundle_map_t& bundle_map)
{
    int is_bundle_creation = 0;
    char* current_bundle_name;
    child_process_t* current_child_processes;

    while (true)
    {
        parsed_input parsed_input;
        std::string line;

        // parse input
        std::getline(std::cin, line);
        line.append("\n");
        parse(line.c_str(), is_bundle_creation, &parsed_input);

        if (is_bundle_creation)
        {
            if (parsed_input.command.type == PROCESS_BUNDLE_STOP)
            {
                bundle_map[current_bundle_name] = current_child_processes;
                delete[] current_bundle_name;
                current_bundle_name = nullptr;
                current_child_processes = nullptr;
                is_bundle_creation = 0;
            }
            else
            {
                current_child_processes->push_back(parsed_input.argv);
            }
        }
        else
        {
            if (parsed_input.command.type == PROCESS_BUNDLE_CREATE)
            {
                current_bundle_name = parsed_input.command.bundle_name;
                current_child_processes = new child_process_t;
                is_bundle_creation = 1;
            }
            else if (parsed_input.command.type == PROCESS_BUNDLE_EXECUTION)
            {
                execute_bundle_command(bundle_map, parsed_input.command.bundle_count, parsed_input.command.bundles);

                // delete bundle names
                for (int i = 0; i < parsed_input.command.bundle_count; i++)
                {
                    delete[] parsed_input.command.bundles[i].name;
                    if (parsed_input.command.bundles[i].input != nullptr)
                        delete[] parsed_input.command.bundles[i].input;
                    if (parsed_input.command.bundles[i].output != nullptr)
                        delete[] parsed_input.command.bundles[i].output;
                    parsed_input.command.bundles[i].name = nullptr;
                    parsed_input.command.bundles[i].input = nullptr;
                    parsed_input.command.bundles[i].output = nullptr;
                }
            }
            else if (parsed_input.command.type == QUIT)
            {
                break;
            }
        }
    }
}


int main()
{
    bundle_map_t bundle_map;
    loop(bundle_map);
    return 0;
}
