Introduction
************

This documentation contains examples and exercises using the OmpSs-2
programming model.  The main objective of this document is to provide guidance
in learning such programming model and serve as teaching materials in courses
and tutorials. To find more applications using OmpSs-2 please visit our web
site in the following URL:

   http://pm.bsc.es

An on-line version of this document can be found in the following URL:

   http://pm.bsc.es/ompss-2-docs/examples

And finally, all the exercises can be found in a .tar.gz package in the following URL:

   http://pm.bsc.es/ompss-2-docs/examples/oss-2-ee.tar.gz

System configuration
====================

In this section we describe how to tune your configure script and also how to
use it to configure your environment. If you have a pre-configured package you
can skip this section and simply run the Linux command ``source`` using the
provided configure script::

  $source configure

The configure script is used to set all the environment variables you need to
properly execute OmpSs-2 applications. Among other things it contains the PATH
where the system will look for to find the Mercurium source-to-source compiler,
or the MKL installation directory (if available) to run specific OmpSs-2
applications (e.g., the Cholesky kernel).

TO-DO: how to manually configure the suite.

Once you have configured the script you will need to to run the Linux command
``source`` using your configure script as described in the beginning of this
section.

The make utility
================

In software development, Make is a utility that automatically builds executable
programs and libraries from source code by reading files called makefiles which
specify how to derive the target program. Though integrated development
environments and language-specific compiler features can also be used to manage
a build process, Make remains widely used, especially in Unix.

Make searches the current directory for the makefile to use, e.g. GNU make
searches files in order for a file named one of GNUmakefile, makefile, Makefile
and then runs the specified (or default) target(s) from (only) that file.

A makefile consists of rules. Each rule begins with a textual dependency line
which defines a target followed by a colon (:) and optionally an enumeration of
components (files or other targets) on which the target depends. The dependency
line is arranged so that the target (left hand of the colon) depends on
components (right hand of the colon). It is common to refer to components as
prerequisites of the target::

  target [target...] : [component...]
  [<TAB>] command-1
  [<TAB>] command-2
  ...
  [<TAB>] command-n
  [target

Below is a very simple makefile that compiles the ''hello'' program (i.e., the
first target: all). It uses the GCC C compiler (CC) and the compile option
''-g'' (CFLAGS) in order to generate the object file ''hello.o''. Once the
object file has been compiled, it uses again the GCC compiler to link
''hello.o'' into the final executable ''hello'' with no linker option
(LDFLAGS). The makefile also provides a ''clean'' target to remove all the
generated files if the user desires to start over (by running make clean)::

   CC = gcc
   CFLAGS = -g
   LDFLAGS =

   all: hello

   hello: hello.o
      # Commands start with TAB not spaces !!!
      $(CC) $(LDFLAGS) -o $@ $^

   hello.o: hello.c
      $(CC) $(CFLAGS) -c -o $@ $<

   clean:
      rm -f hello hello.o

Building the examples
=====================

All the examples and exercises come with a makefile (Makefile) already
configured to build, by default, the corresponding main program. You can also
provide the name of the program as a make utility's parameter (e.g., make
cholesky).

But, appart from building the program's binaries, the makefile is also configured
to create several shell script examples to run the corresponding program. Each
exercise usually has two running scripts: one to run a single program execution
(i.e., ''run-once.sh''); the other will execute multiple configurations with 
respect to the number of used CPUs, input data sizes, etc (this script is called
''multirun.sh''). Before submitting any job, make sure all the environment 
variables have the expected values. You can also use them as a reference to 
create your own configurations. Here is an example of the ''run-once.sh'' 
script::

  #!/bin/bash
  [...]

  ./cholesky 4096 512 1

In some cases running scripts will contain also some job scheduler variables 
declared at the top of the file. A job scheduler script must contain a 
series of directives to inform the batch system about the characteristics of
the job.  These directives appear as comments in the job script file and the
syntax will depend on the job scheduler system used.


Controlling Available CPUs
==========================

In order to limit or constraint the available CPUs for an application, the ''taskset'' tool from Unix can be used. It is used, but not limited, to launch applications with a given CPU affinity.

In order to use taskset, simply precede the application's binary with ''taskset'' followed by a list of CPU ids specifying the desired affinity, like so::

  taskset -c 0,2-4 ./application

The example above would run ''application'' with 4 cores: 0, 2, 3, 4.


Tracing
=======

Along with running scripts also comes a ''trace.sh'' file, which can be used
to include all the environment variables needed to get an instrumentation trace
of the execution. The content of this file is as follows::

  #!/bin/bash
  export EXTRAE_CONFIG_FILE=extrae.xml
  export NANOS6_CONFIG_OVERRIDE="version.instrument=extrae"
  $*

Additionally, you will need to change your running script in order to invoke
the program through this ''trace.sh'' script. Although you can also edit your
running script adding all the environment variables related with the
instrumentation, it is preferable to use this extra script to easily change
between instrumented and non-instrumented executions. When in need to
instrument your execution, simply include ''trace.sh'' before the program
invocation. 

Finally, the make utility will generate (if not already present in the
directory) other configuration files as is the case of the ''extrae.xml''
file (used to configure the Extrae library to get a Paraver trace, see
''trace.sh'' file).


Dependency Graphs
=================

Nanos6 allows for a graphical representation of data dependencies to be extracted. In order to generate said graph, run the application with the ''NANOS6_CONFIG_OVERRIDE'' environment variable set to ''version.instrument=graph''::

  NANOS6_CONFIG_OVERRIDE="version.instrument=graph" ./application
  
By default graph nodes will include the full path of the source code. To remove these, add the `instrument.graph.shorten_filenames=true` option to ''NANOS6_CONFIG_OVERRIDE'' environment variable. Please do not overwrite the environment variable since you would lose the options previously set. You can append an option by running the application in this way::

  NANOS6_CONFIG_OVERRIDE="$NANOS6_CONFIG_OVERRIDE,instrument.graph.shorten_filenames=true" ./application

The result will be a PDF file with several pages, each representing the graph at a certain point in time. For best results, we suggest to display the PDF with "single page" view, showing a full page and to advance page by page.


Obtaining Statistics
====================

Another equally interesting feature of Nanos6 is obtaining statistics. To do so, simply run the application with the ''NANOS6_CONFIG_OVERRIDE'' environment variable set to either ''version.instrument=stats'' or ''version.instrument=stats-papi''. The first collects timing statistics while the second also records hardware counters (compilation with PAPI is needed for the second). By default, the statistics are emitted standard error when the program ends.


Job Scheduler: Minotauro
========================

The current section has a short explanation on how to use the job scheduler systems installed in
BSC's Minotauro machine. Slurm is the utility used in this machine for batch processing support,
so all jobs must be run through it. These are the basic directives to submit jobs:

  * mnsubmit job_script: submits a ''job script'' to the queue system (see below for job script
    directives).
  * mnq: shows all the submitted jobs.
  * mncancel <job_id>: remove the job from the queue system, cancelling the execution of the
    processes, if they were still running.

A job must contain a series of directives to inform the batch system about the characteristics of
the job. These directives appear as comments in the job script, with the following syntax::

   # @ directive = value.

The job would be submitted using: ''mnsubmit <job_script>''. While the jobs are queued, you can check
their status using the command ''mnq'' (it may take a while to start executing). Once a job has been
executed you will get two files. One for the console standard output (.out extension) and another
for the console standard error (.err extension).

Job Scheduler: Marenostrum
==========================

LSF is the utility used at MareNostrum IV for batch processing support, so all jobs must be run
through it. This section provides information to get started with job execution at the Cluster.
These are the basic commands to submit, control and check your jobs:

  * bsub < job_script: submits a ''job script'' passed through standard input (STDIN) to the queue
    system.
  * bjobs: shows all the submitted jobs
  * bkill <job_id>: remove the job from the queue system, canceling the execution of the processes,
    if they were still running.
  * bsc_jobs: shows all the pending or running jobs from your group.
