#ifndef JOB_SYSTEM_VISIONGL
#define JOB_SYSTEM_VISIONGL

#include "Game_Scenes/Object_Controllers.h"

void Execute_Control_Function(void* Parameter)
{
	Controller* Control = (Controller*)Parameter;
	Control->Control_Function();
}

void Execute_Void_Function(void* Parameter)
{
	void(*Function)() = (void(*)())Parameter;

	Function();
}

namespace Job_System
{
	bool Part_Time_Work()
	{
		for (uint8_t W = 0; W < Workers.size(); W++)
		{
			Workers[W].Job_Pool_Lock.lock();
			if (Workers[W].Job_Pool.size())
			{
				Job Selected_Job = Workers[W].Job_Pool.back();
				Workers[W].Job_Pool.pop_back();
				Workers[W].Job_Pool_Lock.unlock();

				Selected_Job.Job_Function(Selected_Job.Parameters);

				return true;
			}

			Workers[W].Job_Pool_Lock.unlock();
		}

		return false;
	}

	/*bool Currently_Working[NUMBER_OF_WORKERS];
	bool Still_Working()
	{
		for (size_t W = 0; W < NUMBER_OF_WORKERS; W++)
			if (Currently_Working[W])
				return true;

		return false;
	}*/

	void Begin_Work(Worker* Worker)
	{
		while (!Working);

		while (Working) // Check if we no longer need to do our jobs
		{
			uint8_t W = Worker->Worker_Index;

			Job Selected_Job;

			Workers[W].Job_Pool_Lock.lock(); // Lock this so we can safely check!

			while (!Workers[W].Job_Pool.size()) // If this job pool doesn't have spare jobs?
			{
				// Currently_Working[Worker->Worker_Index] = false;

				Workers[W].Job_Pool_Lock.unlock(); // We can unlock this current job pool because we won't be using it
				W++;
				W %= NUMBER_OF_WORKERS;
				Workers[W].Job_Pool_Lock.lock(); // But lock the next one while we check it!
			}

			Selected_Job = Workers[W].Job_Pool.back();	// Load up a job!
			Workers[W].Job_Pool.pop_back();				// This is our job now- we can remove it from the list
			Workers[W].Job_Pool_Lock.unlock();			// We don't need this job pool anymore right now- unlock it!

			// Currently_Working[Worker->Worker_Index] = true;

			Selected_Job.Job_Function(Selected_Job.Parameters); // This executes the selected task

			// Work endlessly...
		}
	}

	void Submit_Job(Job Task)
	{
		Last_Job_Submitted_Mutex.lock();

		size_t Index = Last_Job_Submitted_Index;

		Last_Job_Submitted_Index++;
		Last_Job_Submitted_Index %= NUMBER_OF_WORKERS;

		Last_Job_Submitted_Mutex.unlock();

		Workers[Index].Job_Pool_Lock.lock();

		Workers[Index].Job_Pool.push_back(Task);

		Workers[Index].Job_Pool_Lock.unlock();

		//
	}
}

#define Estimated_Worker_Workload 22

void Initialise_Job_System()
{
	if (!Job_System::Working)
	{
		Job_System::Last_Job_Submitted_Index = 0;

		Job_System::Worker_Threads.reserve(NUMBER_OF_WORKERS);

		for (uint8_t W = 0; W < Job_System::Workers.size(); W++)
		{
			Job_System::Workers[W].Worker_Index = W; // Sets worker index

			Job_System::Workers[W].Job_Pool.reserve(Estimated_Worker_Workload); // This makes sure that the job pool can safely handle the estimated worker workload ^^

			Job_System::Worker_Threads.push_back(std::thread(Job_System::Begin_Work, &Job_System::Workers[W])); // This creates a new thread that begins working
		}

		Job_System::Working = true; // This will let the workers know that they can begin working

		// This sets up all of the workers - if they haven't been set up already
	}
}

void Handle_Scene()
{
	while (Job_System::Part_Time_Work()) { ; }

	for (size_t W = 0; W < Scene_Models.size(); W++)
	{
		if (Scene_Models[W]->Flags[MF_UPDATE_MESH])
		{
			Scene_Models[W]->Mesh.Bind_Buffer();
			Scene_Models[W]->Mesh.Update_Vertices();
		}

		if (Scene_Models[W]->Flags[MF_ACTIVE])
			Job_System::Submit_Job(Job_System::Job(Execute_Control_Function, Scene_Models[W]->Control));
	}
}

#endif