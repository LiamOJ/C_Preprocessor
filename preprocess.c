/*
Author: Liam O'Neill Jablonski, 40405608
Date of Last Change: 01/03/2019
Program Purpose: To imitate the first few steps of a C preprocessor and parse a .c file to remove
comments, count non-empty lines, count comments, replace header file calls with content and replace constants with values.
*/

#include <stdio.h>
#include <string.h>	
#include <ctype.h>
#define BUFFER_SIZE 1000 //sets max size of each line read in from file
#define OUTPUT_FILE_EXT ".o"
#define KEY_VALUE_SIZE 50 //sets string size of keys and values used in 'map'.
#define MAX_MAP_ELEMENT_NUMBER 100 //max number of #define elements
#define CLI_COMMENT_INPUT "-c"
#define CLI_INPUT_FILE_INDICATOR "-i"

/*
Purpose: To count comments
Paramters: 
line: is the line read in from file using fgets. 
comment counter: holds the number of comments.  
*/
void comment_count(char *line, int *comment_counter)
{
	for(int loop = 0; loop < strlen(line) ; loop++)
			if(line[loop] == '/' && line[loop+1] == '/')
				++*comment_counter;
}	

/*
Purpose: To count non-empty lines
Paramters: 
line: is the line read in from file using fgets. 
line counter: holds the number of non-empty lines 
*/
void non_empty_line_counter(char *line, int *line_counter)
{
	for(int loop = 0; loop < strlen(line) ; ++loop)
		if(!isspace(line[loop]))
		{
			++*line_counter;
			break;
		}
}

/*
Purpose: To replace preprocessor include commands with full header file content. 
Paramters: 
line: is the line read in from file using fgets. 
output file: this is the file to be written to as a final result.  
*/
void header_replacer(char *line, FILE *output_file)
{
	//Local variables
	char new_lines[] = "\n\n";
	char line_copy[BUFFER_SIZE];
	const char quote[2] = "\""; //used to delineate for header file name
	char header_line [BUFFER_SIZE];
	
	//Copy line, work with copy.
	strcpy(line_copy,line);
	
	//Go into line, check char by char
	for(int header_replace_loop = 0; header_replace_loop < strlen(line_copy);++header_replace_loop)
	{
		//Continue over any whitespaces at start of line
		if(isspace(line_copy[header_replace_loop]))
			continue;
		
		//Break if first non-whitespace isn't a preprocessor command start.
		if(line_copy[header_replace_loop] != '#')
			break;
		
		//This will then isolate the substring between the first and second " in line copy
		//The result is the name of the header file to be opened.
		char *seperated_text;
		seperated_text = strtok(line_copy,quote); //not needed.
		seperated_text = strtok(NULL, quote);
		if(seperated_text != NULL)
		{
			//Open the file with name from above for reading.
			FILE *header_file = fopen(seperated_text,"r");	
			while(fgets(header_line, BUFFER_SIZE, header_file) != NULL)
				fputs(header_line, output_file);
			
			//puts seperation between header files in output
			fputs(new_lines, output_file);
			fclose(header_file);
		}
	} //end of header replace loop
	

}


/*
Purpose: create 'map' of key/value pairs for define replacements, replaces constants, excludes comments (conditionally) and preprocessor command lines.  
Paramters: 
line: is the line read in from file using fgets. 
output file: this is the file to be written to as a final result. 
CLI comment counter: hold flag to indicate if comments should be printed in output file or not 
*/
void main_parser(char *line, FILE *output_file, int *cli_comment_flag)
{
		
	//Main parser local variables
	char define_creation_trigger[] = "#define";
	static int map_number = 0;
	
	//create struct prototype, this will store key and value pairs. Typedef to 'map'. 
	typedef struct maps
	{
	 char struct_key[KEY_VALUE_SIZE];
	 char struct_value[KEY_VALUE_SIZE];
	} map;
	
	//Create array to store pointers to maps 
	map map_array[MAX_MAP_ELEMENT_NUMBER];
	
	//Restricts addition to map for #define lines only
	if(strstr(line, define_creation_trigger) != NULL)
	{
		//Local variables
		char *temp_key_pointer;
		char *temp_value_pointer;
		char temp_key[KEY_VALUE_SIZE];
		char temp_value[KEY_VALUE_SIZE];
		char space[] = " "; //literal space used for tokenisation
		
		//Tokenisation
		strtok(line,space); //first token, not needed = #define
		temp_key_pointer = strtok(NULL,space); // second token, this is the key
		temp_value_pointer = strtok(NULL,space); //third token, this is value
		
		//Removes newline from second half of string, that contains 'value' 
		//Reference: copied this from string_functions.c
	    int len = strlen(temp_value_pointer);
		if(len > 1 && temp_value_pointer[len -1] == '\n')
		{
			temp_value_pointer[len -1] = '\0' ;
		}
		
		//Assigning the tokenised strings to the structs
		strcpy(map_array[map_number].struct_key,temp_key_pointer);
		strcpy(map_array[map_number].struct_value,temp_value_pointer);
		
		//Increment to next struct
		++map_number;
	}
	 
	//Purpose: tokenise the read in line based on the token (the map key) and replace with
	//the map value. 
	for(int tokenisation_loop = 0; tokenisation_loop < map_number; ++tokenisation_loop)
	{	
		//Required variables
		char key[KEY_VALUE_SIZE];
		char value[KEY_VALUE_SIZE]; 
		char search_key[KEY_VALUE_SIZE]; // string used to search line using strstr
		char space1[KEY_VALUE_SIZE] = " "; //A space is added either side of the search param to aid tokenisation
		char space2[KEY_VALUE_SIZE] = " "; // same as above
		
		//Assign key and value from map array to search placeholders
		strcpy(key, map_array[tokenisation_loop].struct_key);
		strcpy(value, map_array[tokenisation_loop].struct_value);

		//Create a search key using key and spaces (to aid tokenisation)
		strcat(space1,key); // put " " and then key
		strcpy(search_key,space1); // copy above to search key
		strcat(search_key,space2); // add space to above.

		//Copy of line made to manipulate in #define manipulation
		char copy_of_line[BUFFER_SIZE];
		strcpy(copy_of_line,line);
	
		//This restricts entry to lines that contain the search key
		if(strstr(copy_of_line,search_key) != NULL)
		{
			
			//required variables
			char second_half_of_line_string[BUFFER_SIZE];
			char *first_half_pointer;
			char *second_half_pointer;
			
			//This deliniates the 2 halves of a line based on the key
			//It inserts the value in the keys place and cats the line back together.
			first_half_pointer = strtok(copy_of_line,key);
			second_half_pointer = strtok(NULL,key);
			strcpy(second_half_of_line_string,second_half_pointer);
			strcat(first_half_pointer,value);
			strcpy(copy_of_line,first_half_pointer);
			strcat(copy_of_line,second_half_of_line_string);
			strcpy(line,copy_of_line);
			
		}
	} //end of tokenisation loop
	
	//Purpose: excludes comments (conditionally) and preprocessor commands
	for(int loop = 0; loop < strlen(line) ; loop++)
	{	
		if(line[loop] == '/' && line[loop+1] == '/' && *cli_comment_flag == 0)
			while(line[loop] != '\n')
				++loop; //iterate over inline comments until line end.
					
		if(line[loop] == '#')
			break;
		
		//This writes to file, char by char
		fputc(line[loop], output_file);
	}
	
}

int main(int argc, char **argv)
{
	//Variables local to main
	int new_line_counter = 0;
	char new_line[] = "\n";
	int line_counter = 0;
	int comment_counter = 0;
	char user_file_name[BUFFER_SIZE];
	char user_output_file_name[BUFFER_SIZE];
	int cli_comment_flag = 0; //0 is no comments, 1 is keep comments 
	char line[BUFFER_SIZE];
	char cli_comment_indicator[10] = "No";
	char confirmation[] = "Yes";
	
	
	//Purpose: collect comment flag and input and output file names.
	for(int main_loop = 0; main_loop < argc; ++main_loop)
	{
		//Local variables
		char *temp_buffer;
		const char dot[1] = ".";
		
		//check if user wants comments:
		if(!strcmp(argv[main_loop],CLI_COMMENT_INPUT))
			cli_comment_flag = 1; //is Flag: if 1 then keep comments in output.
		
		//collect user input filename. 
		if(!strcmp(argv[main_loop],CLI_INPUT_FILE_INDICATOR))
		{
			temp_buffer = argv[main_loop+1];
			strcpy(user_file_name,temp_buffer);
		}
		
		//create output file name from input file name
		if(!strcmp(argv[main_loop],CLI_INPUT_FILE_INDICATOR))
		{
			strcpy(user_output_file_name,argv[main_loop+1]);
			temp_buffer = strtok(user_output_file_name,dot);
			strcat(temp_buffer,OUTPUT_FILE_EXT);
			strcpy(user_output_file_name,temp_buffer);
		}
	}
	
	//open input file and output file
	FILE *input_file = fopen(user_file_name, "r");
	FILE *output_file = fopen(user_output_file_name,"a");
	
	//This is the main working loop, it reads the input file in line by line
	while(fgets(line, BUFFER_SIZE, input_file) != NULL)
	{
		//comment counter
		comment_count(line, &comment_counter);
		
		//non-empty line counter
		non_empty_line_counter(line, &line_counter);
		
		//header replacer
		header_replacer(line, output_file);
		
		//Perform parsing and file writing
		main_parser(line, output_file, &cli_comment_flag);		
	}
	
	if(cli_comment_flag == 1)
		strcpy(cli_comment_indicator,confirmation);
	
	//print to command line.
	printf("\nPreprocessor working...\n");
	printf("\nInput file: %s\nOutput file: %s\n",user_file_name, user_output_file_name);
	printf("\nComments included: %s\n\n", cli_comment_indicator);
	printf("There are %d lines total\n", line_counter);
	printf("There are %d comments total\n", comment_counter);
	printf("\nPreprocessor complete. \n\nExiting...");
	
	//close files
	fclose(input_file);
	fclose(output_file);
	
	return 0;
}