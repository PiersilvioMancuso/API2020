//TODO: DYNAMIC ARRAY FOR THE STATE
// IMPLEMENTING THE COMMANDS (DELETE, PRINT, CHANGE)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct editor Editor;
typedef struct file_state State;
typedef struct dictionary Dictionary ;

/**Data is the struct that represents a string that will be stored into the dictionary
 * @field string is the string stored
 * @field time is the time when it has been stored
 */
struct data{
    char* string;
    long time;
};

typedef struct data Data;

/**Dictionary is the struct that contains all the Data stored
 * @field max_length is a long that represents the maximum length of the dictionary
 * @field current_index is a long that represents the index to where the next data will be stored
 */
struct dictionary{
    long max_length;
    long current_index;
    Data* list_of_strings;
};


struct file_state{
    long time;
    long size;
    long* file;
};


struct editor{
    long max_time;
    long time_size;
    long current_time;
    long current_index;
    State** file_states;

};

typedef struct command{
    char comm;
    long first;
    long second;
} Command;


// -------------- UTILITY ----------------------

/**Get the data of the command on input
 * @param string is the input string
 * @return the command object that contains all the data
 */
Command get_command(char* string){
    char comm = string[strlen(string) -2];
    long first = 0, second = 0;
    if (strlen(string) > 1){
        first = atol(string);
        while (string[0] != ','){
            string = string + 1;
        }
        string = string + 1;
        second = atol(string);
    }
    Command res;
    res.comm = comm;
    res.first = first;
    res.second = second;

    return res;
}

/**Get the maximum value between first and second
 * @param first is a long value
 * @param second is a long value
 * @return the maximum value
 */
long max_long(long first, long second){
    if (second < first) {
        return first;
    } else {
        return second;
    }
}

/**Get the maximum value between first and second
 * @param first is a long value
 * @param second is a long value
 * @return the maximum value
 */
long min_long(long first, long second){
    if (second > first) {
        return first;
    } else {
        return second;
    }
}




// ------------- DICTIONARY ----------------

void add_string(char* string, Dictionary* dict, long time){
    if (dict->current_index >= dict->max_length){
        dict->max_length = (long) (dict->max_length * 1.5);
        dict->list_of_strings = (Data*) realloc(dict->list_of_strings, dict->max_length * sizeof(Data));
    }
    dict->list_of_strings[dict->current_index].string = string;
    dict->list_of_strings[dict->current_index].time = time;
    dict->current_index++;

}

/*
void print_all_strings(Dictionary *dict){
    for (long i = 0; dict->list_of_strings[i].string != NULL; i++) {
        printf("%s ------ %d\n", dict->list_of_strings[i].string, dict->list_of_strings[i].time);
    }
}*/


// ------------- STATE ----------------------------

void set_strings(State* state, long* array, long time, long size){
    state->size = size;
    state->time = time;
    state->file = array;
}



void update_change_state(State* starting_state, State* ending_state, long first_address, long second_address,const long* array, long time){
    free(ending_state->file);
    if (starting_state->size == 0 || starting_state == NULL){
        ending_state->size = second_address;
        ending_state->file = (long*) calloc(ending_state->size, sizeof(long));

        for (long i = 0; i < second_address; i++) {
            ending_state->file[i] = array[i];
        }
    }

    else{
        ending_state->size = max_long(starting_state->size, second_address);
        ending_state->file = (long*) calloc(ending_state->size, sizeof(long));
        for (long i = 0, k = 0; i < ending_state->size; i++) {
            if (i >= first_address - 1 && i <= second_address -1){
                ending_state->file[i] = array[k];
                k++;
            }
            else ending_state->file[i] = starting_state->file[i];
        }
    }

    ending_state->time = time;
}



void update_delete_state(State* state, State* end_state,long first_address, long second_address, long time){
    int diff = second_address - first_address;
    free(end_state->file);
    if (first_address > state->size){
        end_state->size = state->size;
        end_state->file = (long*) calloc(state->size, sizeof(long));

        for (long i = 0; i < state->size; i++){
            end_state->file[i] = state->file[i];
        }
    }
    else if (first_address == 1 && second_address >= state->size){
        end_state->size = 0;
    }

    else if (first_address <= state->size && second_address >= state->size){
        end_state->size = first_address - 1;
        end_state->file = (long*) calloc(first_address - 1, sizeof(long));

        for (long i = 0; i < first_address - 1; i++) {
            end_state->file[i] = state->file[i];
        }
    }
    else{
        end_state->size = state->size - diff - 1;
        end_state->file = (long*) calloc(end_state->size, sizeof(long));

        for (int i = 0; i < state->size; i++) {
            if (i >= first_address - 1) {
                end_state->file[i] = state->file[i + diff + 1];
            }

            else{
                end_state->file[i] = state->file[i];
            }
        }
    }

    end_state->time = time;
}

void command_print(State* state, Dictionary *dictionary, long first_address, long second_address){
    for (long i = first_address - 1; i < second_address; i++) {
        if (i < state->size){
            printf("%s", dictionary->list_of_strings[state->file[i]].string);
        } else{
            printf(".\n");
        }
    }
}

// -------------------- EDITOR -------------------

void initialize_editor(Editor *edu){
    edu->current_index = 0;
    edu->current_time = 0;
    edu->max_time = 0;
    edu->time_size = 20;
    edu->file_states = (State**) malloc(20 * sizeof(State*));
}

void command_change(Editor * edu, Dictionary *dictionary, long first_address, long second_address, char** pString){
    long time = edu->max_time;
    long current_index = edu->current_index;
    free(edu->file_states[current_index]);
    edu->file_states[current_index] = (State*) calloc(1, sizeof(State));
    edu->file_states[current_index]->size = 0;
    long array[second_address];


    if (edu->current_index == 0){
        edu->file_states[0]->size = second_address;
        edu->file_states[0]->time = edu->max_time;

        if (time != 0){
            free(dictionary->list_of_strings);
            dictionary->current_index = 0;
            dictionary->list_of_strings = (Data*) calloc(dictionary->max_length, sizeof(Data));
        }


    } else if (edu->time_size < edu->current_index + 1){
        edu->time_size = (long) edu->time_size * 1.5;
        edu->file_states = (State**) realloc(edu->file_states,edu->time_size * sizeof(State*));
        edu->file_states[edu->current_index]->size = second_address;
        edu->file_states[edu->current_index]->time = edu->max_time;


    }

    long i = 0;
    for (; i < second_address - first_address +1; i++) {

        array[i] = dictionary->current_index;
        add_string(pString[i], dictionary, edu->current_time);
    }

    if (edu->current_index == 0){

        update_change_state(edu->file_states[0], edu->file_states[0], first_address, second_address, array, edu->current_time);
    }
    else{
        update_change_state(edu->file_states[edu->current_index-1], edu->file_states[edu->current_index], first_address, second_address, array, edu->current_time);

    }



    edu->max_time++;
    edu->current_index++;

    if (strcmp(".\n", pString[i]) == 0) return;
}




int main(){
    Dictionary *dictionary = (Dictionary*) calloc(1, sizeof(Dictionary));
    dictionary->max_length = 10;
    dictionary->current_index = 0;
    dictionary->list_of_strings = (Data*) calloc(10, sizeof(Data));

    Editor * edu = calloc(1, sizeof(Editor));
    initialize_editor(edu);

    char* string = (char*) calloc(1024, sizeof(char));
    while (1){
        string = fgets(string, 1024, stdin);
        Command active = get_command(string);

        if (active.comm == 'q'){
            return 0;
        }

        else if(active.comm == 'p') {
            command_print(edu->file_states[edu->current_index - 1], dictionary, active.first, active.second);

        } else{

            char* array_string[active.second - active.first + 2];

            for (int j = 0; j < active.second - active.first + 2; j++) {
                array_string[j] = strdup(fgets(string, 1024, stdin));
            }

            command_change(edu, dictionary, active.first, active.second, array_string);


            for (long i = 0; i < dictionary->current_index; ++i) {
                printf("%s", dictionary->list_of_strings[i].string);
            }

        }
    }


}