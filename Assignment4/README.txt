WELCOME TO THE WORLD OF TOMORROW!

To make the scheduler program use the command:
	make

To run the project, use the command:
	./scheduler <PROCEDURE> <Workfile> <Round Robin Time>

For both the policy implimentation and the analysis I used a single-linked list as it was more adaptable for this
circumstance than a Array, as arrays are not as malliable. Additionally, each struct has the parameters on the worksheet
but with a timeElapsed int, a startTime int, and an endTime int. These keep track of how much more time the job needs to work,
when the job started, and when the job ended.

The policies are pretty easy
    FIFO just processed the list in the same order it came in

    SJF just sorted the list in ascending order first, then processed the list

    RR took the list and determined how many jobs were there. Once this number has been determined, RR starts paging through
    the list and adding the timeslice to the timeElapsed of each job, then once the timeElapsed is equal to the length of the job,
    the number of jobs remaining is reduced. This continues until there are no jobs remaining.
