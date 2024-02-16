#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Constants for file paths and processor configuration
#define PROGRAM_FILE_PATH "program.txt"
#define DATA_FILE_PATH "data.txt"
#define MEMORY_SIZE 512
#define REGISTER_COUNT 8
#define EMPTY_VALUE 0xFFFF
#define ADD 0b0000
#define SUB 0b0001
#define AND 0b0010
#define OR 0b0011
#define XOR 0b0100
#define LOAD 0b0101
#define STORE 0b0110

// Structure to represent the processor
struct processor
{
    uint16_t memory[MEMORY_SIZE];
    uint16_t registers[REGISTER_COUNT];
    uint16_t program_counter;
} *my_processor = NULL;

// Function prototypes
struct processor *initialize_processor();
void load_program(char *program_file_path);
void load_data(char *data_file_path);
void execute_program();
char *fget_line(char *line, int size, FILE *file);
uint16_t string_to_uint16_t(char *string);
void instruction_decoder(uint16_t instruction);
void data_transfer(uint8_t opcode, uint8_t register_index, uint8_t memory_address);
void arithmetic_logical_unit(uint8_t opcode, uint8_t i_bit, uint8_t instruction_operand1, uint8_t instruction_operand2, uint8_t instruction_operand3);

int main()
{
    // Initialize the processor
    my_processor = initialize_processor();

    // Load program and data into memory
    load_program(PROGRAM_FILE_PATH);
    load_data(DATA_FILE_PATH);

    // Execute the program
    execute_program();

    // Free allocated memory
    free(my_processor);
    my_processor = NULL;

    return 0;
}

// Function to initialize the processor
struct processor *initialize_processor()
{
    struct processor *p1 = (struct processor *)malloc(sizeof(struct processor));
    if (p1 == NULL)
    {
        printf("Error in Memory Allocation for the Processor\n");
        exit(0);
    }

    // Initialize memory to EMPTY_VALUE
    int i = 0;
    while (i < MEMORY_SIZE)
    {
        p1->memory[i++] = EMPTY_VALUE;
    }

    // Initialize registers to 0
    i = 0;
    while (i < REGISTER_COUNT)
    {
        p1->registers[i++] = 0;
    }

    // Initialize program counter to 0
    p1->program_counter = 0;

    return p1;
}

// Function to load the program from a file into memory
void load_program(char *program_file_path)
{
    // Open the program file
    FILE *program_file = fopen(program_file_path, "r");
    if (program_file == NULL)
    {
        printf("Error in Opening the %s File\n", program_file_path);
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }

    // Read each line from the program file and convert it to machine code
    char instruction[20];
    int i = 0;
    while (fget_line(instruction, 20, program_file) != NULL)
    {
        my_processor->memory[i++] = string_to_uint16_t(instruction);
    }

    // Close the program file
    if (fclose(program_file) != 0)
    {
        printf("Error in Closing the %s File\n", program_file_path);
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }
}

// Function to load data from a file into memory
void load_data(char *data_file_path)
{
    // Open the data file
    FILE *data_file = fopen(data_file_path, "r");
    if (data_file == NULL)
    {
        printf("Error in Opening the %s File\n", data_file_path);
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }

    // Read each line from the data file and store data in memory
    char line[20];
    int i = 0;
    uint16_t address, data;
    while (fget_line(line, 20, data_file) != NULL)
    {
        line[4] = '\0';
        if ((address = string_to_uint16_t(line)) >= MEMORY_SIZE)
        {
            printf("Error in %dth Data in %s File: Memory Address exceeds Memory Size %d\n", i + 1, DATA_FILE_PATH, MEMORY_SIZE);
            free(my_processor);
            my_processor = NULL;
            exit(0);
        }

        data = string_to_uint16_t(line + 5);

        my_processor->memory[address] = data;
        i++;
    }

    // Close the data file
    if (fclose(data_file) != 0)
    {
        printf("Error in Closing the %s File\n", data_file_path);
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }
}

// Function to execute the program
void execute_program()
{
    // Execute instructions until program counter reaches the end or encounters an empty value
    uint16_t instruction;
    while (my_processor->program_counter < MEMORY_SIZE && (instruction = my_processor->memory[my_processor->program_counter]) != EMPTY_VALUE)
    {
        my_processor->program_counter++;
        instruction_decoder(instruction);
    }

    // Display the content of all registers at the end of execution
    printf("\n\t--------\n");
    printf("\tRn Value\n");
    printf("\t--------\n");
    int i = 0;
    while (i < REGISTER_COUNT)
    {
        printf("\tR%d %5d\n", i, my_processor->registers[i]);
        i++;
    }
    printf("\t--------\n");
    printf("\n");
}

// Function to read a line from a file
char *fget_line(char *line, int size, FILE *file)
{
    int character;
    int length = 0;

    // Read characters until the end of line or end of file
    while (length < size - 1)
    {
        character = getc(file);

        if (character == '\n' || character == '\r')
        {
            if (length == 0)
            {
                continue;
            }
            else
            {
                break;
            }
        }

        if (character == EOF)
        {
            if (length == 0)
            {
                return NULL;
            }
            else
            {
                break;
            }
        }

        line[length++] = (char)character;
    }

    // Null-terminate the string
    line[length] = '\0';

    return line;
}

// Function to convert a binary string to a uint16_t number
uint16_t string_to_uint16_t(char *string)
{
    int i = 0;
    uint16_t number = 0;
    char c;

    // Convert each character to a binary number
    while ((c = string[i]) != '\0')
    {
        number = number * 2 + (c - '0');
        i++;
    }

    return number;
}

// Function to decode the instruction and perform data transfer or ALU operation
void instruction_decoder(uint16_t instruction)
{
    // Extract various fields from the instruction
    uint8_t operand3 = instruction & 0x07;
    uint8_t operand2 = (instruction >> 3) & 0x0F;
    uint8_t operand1 = (instruction >> 7) & 0x07;
    uint8_t i_bit = (instruction >> 10) & 0x01;
    uint8_t opcode = (instruction >> 11) & 0x0F;
    uint8_t t_bit = (instruction >> 15) & 0x01;

    // Check the type bit and perform data transfer or ALU operation accordingly
    if (t_bit == 1)
    {
        data_transfer(opcode, operand1, operand2);
    }
    else
    {
        arithmetic_logical_unit(opcode, i_bit, operand1, operand2, operand3);
    }
}

// Function to perform data transfer operations (LOAD and STORE)
void data_transfer(uint8_t opcode, uint8_t register_index, uint8_t memory_address)
{
    // Check if memory address is within bounds
    if (memory_address >= MEMORY_SIZE)
    {
        printf("Error in %dth Machine Code in %s File: Memory Address (Operand 2) exceeds Memory Size %d", my_processor->program_counter, PROGRAM_FILE_PATH, MEMORY_SIZE);
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }

    // Perform data transfer operation based on the opcode
    if (opcode == LOAD)
    {
        my_processor->registers[register_index] = my_processor->memory[memory_address];
    }
    else if (opcode == STORE)
    {
        my_processor->memory[memory_address] = my_processor->registers[register_index];
    }
    else
    {
        printf("Error in %dth Machine Code in %s File: ", my_processor->program_counter, PROGRAM_FILE_PATH);
        printf("T-Bit is 1 But OPCode isn't a Data Transfer Operation\n");
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }
}

// Function to perform ALU operations (ADD, SUB, AND, OR, XOR)
void arithmetic_logical_unit(uint8_t opcode, uint8_t i_bit, uint8_t instruction_operand1, uint8_t instruction_operand2, uint8_t instruction_operand3)
{
    uint16_t operand1, operand2 = my_processor->registers[instruction_operand3];

    // Determine the value of operand1 based on the immediate bit
    if (i_bit == 1)
    {
        operand1 = instruction_operand2;
    }
    else
    {
        // Check if register index is within bounds
        if (instruction_operand2 > 0b0111)
        {
            printf("Error in %dth Machine Code in %s File: ", my_processor->program_counter, PROGRAM_FILE_PATH);
            printf("Register Index (Operand 2) is Out Of Bound\n");
            free(my_processor);
            my_processor = NULL;
            exit(0);
        }

        operand1 = my_processor->registers[instruction_operand2];
    }

    // Perform the ALU operation based on the opcode
    switch (opcode)
    {
    case ADD:
        my_processor->registers[instruction_operand1] = operand1 + operand2;
        break;

    case SUB:
        my_processor->registers[instruction_operand1] = operand1 - operand2;
        break;

    case AND:
        my_processor->registers[instruction_operand1] = operand1 & operand2;
        break;

    case OR:
        my_processor->registers[instruction_operand1] = operand1 | operand2;
        break;

    case XOR:
        my_processor->registers[instruction_operand1] = operand1 ^ operand2;
        break;

    default:
        printf("Error in %dth Machine Code in %s File: ", my_processor->program_counter, PROGRAM_FILE_PATH);
        printf("T-Bit is 0 But OPCode isn't an ALU Operation\n");
        free(my_processor);
        my_processor = NULL;
        exit(0);
    }
}