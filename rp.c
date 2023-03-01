#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means candidate i won over candidate j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
char *candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, char *name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

bool cycle(int winner, int loser)
{
    if (locked[loser][winner])
    {
        return true;
    }
    for (int j =0; j < candidate_count; j++)
    {
        if (locked[loser][j] && cycle(winner, j))
        {
            return true;
        }
    }
    return false;
}


int main()
{
    // Populate array of candidates
    printf("Num of candidates: ");
    scanf("%i", &candidate_count); 
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        char *tmp = malloc(sizeof(char) * 25);
        printf("Name of Candidate %i :", i + 1);
        scanf("%s", tmp);
        candidates[i] = tmp;
    }

    //set all preferences to zero
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            preferences[i][j] = 0;
        }
    }


    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    printf("Number of voters: ");
    int voter_count = 0;
    scanf("%i", &voter_count);

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            char *name = malloc(sizeof(char) * 30);
            printf("Rank %i: ", j + 1);
            scanf("%s", name);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    /*for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            printf("%i\n", preferences[i][j]);
        }

    }*/  //debugging tool


    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, char* name, int ranks[])
{
    for (int k = 0; k < candidate_count; k++)
    {
        if (strcmp(candidates[k], name) == 0)
        {
            ranks[rank] = k;
            return true;
        }

    }

    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int m = 0; m < candidate_count; m++)
    {
        for (int k = 0; k < candidate_count; k++)
        {
            if (ranks[m] == k)
            {
                for (int i = m; i < candidate_count; i++)
                {
                    preferences[k][ranks[i]]++;
                }
                preferences[k][k] = 0;

                break;
            }

        }


    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] - preferences[j][i] > 0)
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }

    }

    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    //selection sorting:
    int difference[pair_count];
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] - preferences[j][i] > 0)
            {
                difference[i] = preferences[i][j] - preferences[j][i];
            }
        }

    }


    for (int i = 0; i < pair_count; ++i)
    {
        for (int j = i + 1; j < pair_count; ++j)
        {
            if (difference[i] < difference[j])
            {
                pair xyz = pairs[i];
                pairs[i] = pairs[j];
                pairs[j] = xyz;
            }
        }
    }
    //printf("%s by %i vs %s %s by %i vs %s %s by %i vs %s", candidates[pairs[0].winner], pairs[0].difference, candidates[pairs[0].loser], candidates[pairs[1].winner], pairs[1].difference, candidates[pairs[1].loser], candidates[pairs[2].winner], pairs[2].difference, candidates[pairs[2].loser]);
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    /*  //commented code allows cycles to form as long as one source is still remaining.
    bool cycle = false;
    for (int i = 0; i < candidate_count; i++)
    {
        int checker = 0;
        for (int x = 0; x < candidate_count; x++)
        {
            for (int y = 0; y < candidate_count; y++)
            {
                if (locked[y][x])
                {
                    checker++;
                    break;
                }

            }
            if (checker == candidate_count)
            {
                cycle = true;
                printf("cycle");
                break;
            }
        }

        if (!cycle)
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }


    }*/

    for (int i = 0; i < pair_count; i++)
    {
        if (!cycle(pairs[i].winner, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }




    return;
}

// Print the winner of the election
void print_winner(void)
{
    char win;
    for (int x = 0; x < candidate_count; x++)
    {
        int false_count = 0;
        for (int y = 0; y < candidate_count; y++)
        {
            if (locked[y][x])
            {
                break;
            }
            false_count++;
        }
        if (false_count == candidate_count)
        {
            printf("winner is %s !!!!\n", candidates[x]);
            break;
        }
    }


    return;
}
