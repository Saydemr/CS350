#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int num_people;
    printf("Insert the number of people on the table (int): \n");
    scanf("%d", &num_people);

    int preferences[5];
    for (int i = 0; i < 5; ++i)
        preferences[i] = 0;

    char meals[5][10] = {"Kabab","Burger","Pasta","Lahmacun","Salad" };


    printf("*** FIRST ROUND ***\n");
    for (int i = 0; i < num_people ; ++i)
    {
        printf("Dear guest #%d\n",(i+1));
        printf("MENU\n");
        for (int i = 0; i < 5 ; ++i)
            printf("%d-%s\n", (i+1), meals[i]);

        for (int j = 0; j < 5; ++j)
        {

            printf("Please enter the item you prefer %d. : ",(j+1));
            int user_preference;
            scanf("%d", &user_preference);

            preferences[user_preference-1] += 5-j;
        }
        printf("\n\n");
    }

    int threshold = 10;
    int short_list_size = 0;

    for (int i = 0; i < 5; ++i)
        if (preferences[i] > threshold)
            short_list_size++;

    if (short_list_size < 1)
        printf("\nYou are eating at home/dorm today!\n");
    else if (short_list_size < 2)
    {
        for (int i = 0; i < 5; ++i)
            if (preferences[i] > threshold)
                printf("\nEveryone prefers : %s\n", meals[i]);
    }
    else 
    {
        printf("*** SECOND ROUND ***\n");
        char* short_listed_meals[short_list_size][10];
        int pointer = 0;

        int preferences_short_listed[short_list_size];
        for (int i = 0; i < short_list_size; ++i)
            preferences_short_listed[i] = 0;

        for (int i = 0; i < 5; ++i)
            if (preferences[i] > threshold)
                strcpy(short_listed_meals[pointer++], meals[i]);

        printf("\nRemaining Items in the Menu\n");
        for (int i = 0; i < short_list_size ; ++i)
            printf("%d-%s\n", (i+1), short_listed_meals[i]);
        
        for (int i = 0; i < num_people ; ++i)
        {
            printf("\nDear guest #%d\n",(i+1));
            for (int j = 0; j < short_list_size; ++j)
            {
                printf("Please enter the item you prefer %d. : ",(j+1));
                int user_preference;
                scanf("%d", &user_preference);

                preferences_short_listed[user_preference-1] += 5-j;
            }
        }

        int max = -1;        
        int max_index = -1;
        for (int i = 0; i < short_list_size; ++i)
        {
            if (preferences_short_listed[i] > max)
            {
                max = preferences_short_listed[i];
                max_index = i;
            }
        }
            
        printf("\nEveryone eats %s\n", short_listed_meals[max_index]);
        printf("Scores for meals are as follows\n");
        for (int i = 0; i < short_list_size; ++i)
            printf("%d-%s : %d\n", (i+1), short_listed_meals[i], preferences_short_listed[i]);

    }
    return 4;
}
