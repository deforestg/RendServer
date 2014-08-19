//============================================================================
// Name        : RendServer.cpp
// Author      : Gabriel de Forest
// Version     :
// Copyright   : No copyright
// Description : Rend Server main file
//============================================================================

#include "include/TcpServer.h"
#include "include/UdpServer.h"
#include <syslog.h>

int daemonize() {
	// Fork the process and have the parent exit. If the process was started
	// from a shell, this returns control to the user. Forking a new process is
	// also a prerequisite for the subsequent call to setsid().
	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		} else {
			syslog(LOG_ERR | LOG_USER, "First fork failed: %m");
			return 1;
		}
	}

	// Make the process a new session leader. This detaches it from the
	// terminal.
	setsid();

	// A process inherits its working directory from its parent. This could be
	// on a mounted filesystem, which means that the running daemon would
	// prevent this filesystem from being unmounted. Changing to the root
	// directory avoids this problem.
	chdir("/");

	// The file mode creation mask is also inherited from the parent process.
	// We don't want to restrict the permissions on files created by the
	// daemon, so the mask is cleared.
	umask(0);

	// A second fork ensures the process cannot acquire a controlling terminal.
	if (pid_t pid = fork()) {
		if (pid > 0) {
			exit(0);
		} else {
			syslog(LOG_ERR | LOG_USER, "Second fork failed: %m");
			return 1;
		}
	}

	// Close the standard streams. This decouples the daemon from the terminal
	// that started it.
	close(0);
	close(1);
	close(2);

	// We don't want the daemon to have any standard input.
	if (open("/dev/null", O_RDONLY) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to open /dev/null: %m");
		return 1;
	}

	// Send standard output to a log file.
	const char* output = "/var/log/rend/rend.out";
	const int flags = O_WRONLY | O_CREAT | O_APPEND;
	const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

	if (open(output, flags, mode) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to open output file %s: %m", output);
		return 1;
	}

	// Also send standard error to the same log file.
	if (dup(1) < 0) {
		syslog(LOG_ERR | LOG_USER, "Unable to dup output descriptor: %m");
		return 1;
	}

	return 0;
}

int main() {
	try {
	    boost::asio::io_service ioService;
//	    TcpServer tcpServer(ioService);
	    UdpServer udpServer(ioService);

		// Register signal handlers so that the daemon may be shut down. You may
		// also want to register for other signals, such as SIGHUP to trigger a
		// re-read of a configuration file.
		boost::asio::signal_set signals(ioService, SIGINT, SIGTERM);
		signals.async_wait(
			boost::bind(&boost::asio::io_service::stop, &ioService)
		);

		// Inform the io_service that we are about to become a daemon. The
		// io_service cleans up any internal resources, such as threads, that may
		// interfere with forking.
		ioService.notify_fork(boost::asio::io_service::fork_prepare);

		if (daemonize() != 0) {
			return 1;
		}

		// Inform the io_service that we have finished becoming a daemon. The
		// io_service uses this opportunity to create any internal file descriptors
		// that need to be private to the new process.
		ioService.notify_fork(boost::asio::io_service::fork_child);

		// The io_service can now be used normally.
		syslog(LOG_INFO | LOG_USER, "Daemon started");
		ioService.run();
		syslog(LOG_INFO | LOG_USER, "Daemon stopped");
	} catch (std::exception& e) {
		syslog(LOG_ERR | LOG_USER, "Exception: %s", e.what());
		std::cerr << "Exception: " << e.what() << std::endl;
	}

	return 0;
}
