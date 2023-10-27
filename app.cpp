// Define a structure for tasks
struct task {
    int id; // A unique identifier for the task
    int difficulty; // A measure of how difficult the task is
    int urgency; // A measure of how urgent the task is
    int assigned_to; // The id of the team member who is assigned to the task
};

// Define a structure for team members
struct team_member {
    int id; // A unique identifier for the team member
    int skill_level; // A measure of how skilled the team member is
    int availability; // A measure of how available the team member is
    int workload; // A measure of how many tasks the team member has
    struct task* tasks; // A pointer to an array of tasks assigned to the team member
};

// Define a function to compare two tasks based on their difficulty and urgency
int compare_tasks(struct task* t1, struct task* t2) {
    // If t1 has higher urgency than t2, return 1
    if (t1->urgency > t2->urgency) {
        return 1;
    }
    // If t1 has lower urgency than t2, return -1
    else if (t1->urgency < t2->urgency) {
        return -1;
    }
    // If t1 and t2 have equal urgency, compare their difficulty
    else {
        // If t1 has higher difficulty than t2, return 1
        if (t1->difficulty > t2->difficulty) {
            return 1;
        }
        // If t1 has lower difficulty than t2, return -1
        else if (t1->difficulty < t2->difficulty) {
            return -1;
        }
        // If t1 and t2 have equal difficulty, return 0
        else {
            return 0;
        }
    }
}

// Define a function to compare two team members based on their skill level and availability
int compare_team_members(struct team_member* tm1, struct team_member* tm2) {
    // If tm1 has higher skill level than tm2, return 1
    if (tm1->skill_level > tm2->skill_level) {
        return 1;
    }
    // If tm1 has lower skill level than tm2, return -1
    else if (tm1->skill_level < tm2->skill_level) {
        return -1;
    }
    // If tm1 and tm2 have equal skill level, compare their availability
    else {
        // If tm1 has higher availability than tm2, return 1
        if (tm1->availability > tm2->availability) {
            return 1;
        }
        // If tm1 has lower availability than tm2, return -1
        else if (tm1->availability < tm2->availability) {
            return -1;
        }
        // If tm1 and tm2 have equal availability, return 0
        else {
            return 0;
        }
    }
}

// Define a function to assign tasks to team members using a greedy algorithm
void assign_tasks(struct task* tasks, int n_tasks, struct team_member* team_members, int n_team_members) {
    // Sort the tasks in descending order of their difficulty and urgency
    sort(tasks, n_tasks, compare_tasks);

    // Sort the team members in descending order of their skill level and availability
    sort(team_members, n_team_members, compare_team_members);

    // Initialize a variable to keep track of the current task index
    int current_task = 0;

    // Loop through the team members
    for (int i = 0; i < n_team_members; i++) {
        // Get the current team member
        struct team_member* tm = &team_members[i];

        // Loop until the current team member is unavailable or all tasks are assigned
        while (tm->availability > 0 && current_task < n_tasks) {
            // Get the current task
            struct task* t = &tasks[current_task];

            // Assign the current task to the current team member
            t->assigned_to = tm->id;

            // Update the workload and availability of the current team member
            tm->workload += t->difficulty;
            tm->availability -= t->difficulty;

            // Allocate memory for the new task in the array of tasks assigned to the current team member
            tm->tasks = realloc(tm->tasks, (tm->workload + 1) * sizeof(struct task));

            // Copy the current task to the array of tasks assigned to the current team member
            tm->tasks[tm->workload] = *t;

            // Increment the current task index
            current_task++;
        }
    }

    // Check if any team member is overloaded or underloaded
    for (int i = 0; i < n_team_members; i++) {
        // Get the current team member
        struct team_member* tm = &team_members[i];

        // If the current team member is overloaded, try to redistribute some tasks to other team members
        if (tm->availability < 0) {
            // Loop through the tasks assigned to the current team member in ascending order of their difficulty and urgency
            for (int j = 0; j < tm->workload; j++) {
                // Get the current task
                struct task* t = &tm->tasks[j];

                // Loop through the other team members in ascending order of their skill level and availability
                for (int k = i + 1; k < n_team_members; k++) {
                    // Get the other team member
                    struct team_member* tm2 = &team_members[k];

                    // If the other team member has enough availability to take the current task, transfer it to them
                    if (tm2->availability >= t->difficulty) {
                        // Update the assigned_to field of the current task
                        t->assigned_to = tm2->id;

                        // Update the workload and availability of both team members
                        tm->workload -= t->difficulty;
                        tm->availability += t->difficulty;
                        tm2->workload += t->difficulty;
                        tm2->availability -= t->difficulty;

                        // Allocate memory for the new task in the array of tasks assigned to the other team member
                        tm2->tasks = realloc(tm2->tasks, (tm2->workload + 1) * sizeof(struct task));

                        // Copy the current task to the array of tasks assigned to the other team member
                        tm2->tasks[tm2->workload] = *t;

                        // Remove the current task from the array of tasks assigned to the current team member
                        memmove(&tm->tasks[j], &tm->tasks[j + 1], (tm->workload - j) * sizeof(struct task));

                        // Break out of the inner loop
                        break;
                    }
                }

                // If the current team member is no longer overloaded, break out of the outer loop
                if (tm->availability >= 0) {
                    break;
                }
            }
        }

        // If the current team member is underloaded, try to take some tasks from other team members
        else if (tm->availability > 0) {
            // Loop through the other team members in descending order of their skill level and availability
            for (int k = i - 1; k >= 0; k--) {
                // Get the other team member
                struct team_member* tm2 = &team_members[k];

                // Loop through the tasks assigned to the other team member in descending order of their difficulty and urgency
                for (int j = tm2->workload - 1; j >= 0; j--) {
                    // Get the current task
                    struct task* t = &tm2->tasks[j];

                    // If the current team member has enough availability to take the current task, transfer it to them
                    if (tm->availability >= t->difficulty) {