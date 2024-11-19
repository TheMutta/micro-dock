#define _GNU_SOURCE  

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sched.h>
#include <string.h>
#include <sys/socket.h>
#include <assert.h>
#include <sys/un.h>

#define STACK_SIZE (64 * 1024)

int create_container(void *param) {
	const char *rootfs = param;

	char *cmd[] = { "/bin/bash", NULL };

	chroot(param);
	chdir("/");
	mount("proc", "/proc", "proc", 0, "");
	mount("none", "/tmp", "tmpfs", 0, "");
	execv("/bin/bash", cmd);
	perror("exec");

	return EXIT_FAILURE;
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf("Usage: micro-dock <rootfs>\n");
		return EXIT_FAILURE;
	}

	int clone_flags = CLONE_NEWIPC |
		CLONE_NEWNET |
		CLONE_NEWPID |
		CLONE_NEWNS |
		CLONE_NEWUTS |
		SIGCHLD;

	pid_t pid = 
		clone(create_container, malloc(STACK_SIZE) + STACK_SIZE, clone_flags, argv[1]);

	waitpid(pid, NULL, 0);

	return EXIT_SUCCESS;

}
