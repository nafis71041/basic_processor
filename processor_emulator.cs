using System;
using System.IO;

class Processor
{
    const string PROGRAM_FILE_PATH = "program.txt";
    const string DATA_FILE_PATH = "data.txt";
    const int MEMORY_SIZE = 512;
    const int REGISTER_COUNT = 8;
    const ushort EMPTY_VALUE = 0xFFFF;
    const byte ADD = 0b0000;
    const byte SUB = 0b0001;
    const byte AND = 0b0010;
    const byte OR = 0b0011;
    const byte XOR = 0b0100;
    const byte LOAD = 0b0101;
    const byte STORE = 0b0110;

    ushort[] memory = new ushort[MEMORY_SIZE];
    ushort[] registers = new ushort[REGISTER_COUNT];
    ushort program_counter = 0;

    static Processor my_processor;

    static void Main()
    {
        my_processor = InitializeProcessor();

        LoadProgram(PROGRAM_FILE_PATH);
        LoadData(DATA_FILE_PATH);

        ExecuteProgram();

        my_processor = null;
    }

    static Processor InitializeProcessor()
    {
        var p1 = new Processor();

        for (int i = 0; i < MEMORY_SIZE; i++)
            p1.memory[i] = EMPTY_VALUE;

        for (int i = 0; i < REGISTER_COUNT; i++)
            p1.registers[i] = 0;

        p1.program_counter = 0;

        return p1;
    }

    static void LoadProgram(string program_file_path)
    {
        if (!File.Exists(program_file_path))
        {
            Console.WriteLine($"Error in Opening the {program_file_path} File");
            my_processor = null;
            Environment.Exit(0);
        }

        string[] lines = File.ReadAllLines(program_file_path);
        for (int i = 0; i < lines.Length; i++)
        {
            my_processor.memory[i] = StringToUInt16(lines[i]);
        }
    }

    static void LoadData(string data_file_path)
    {
        if (!File.Exists(data_file_path))
        {
            Console.WriteLine($"Error in Opening the {data_file_path} File");
            my_processor = null;
            Environment.Exit(0);
        }

        string[] lines = File.ReadAllLines(data_file_path);
        for (int i = 0; i < lines.Length; i++)
        {
            string line = lines[i];
            string addr_str = line.Substring(0, 4);
            ushort address = StringToUInt16(addr_str);
            if (address >= MEMORY_SIZE)
            {
                Console.WriteLine($"Error in {i + 1}th Data in {DATA_FILE_PATH} File: Memory Address exceeds Memory Size {MEMORY_SIZE}");
                my_processor = null;
                Environment.Exit(0);
            }

            ushort data = StringToUInt16(line.Substring(5));
            my_processor.memory[address] = data;
        }
    }

    static void ExecuteProgram()
    {
        ushort instruction;
        while (my_processor.program_counter < MEMORY_SIZE &&
               (instruction = my_processor.memory[my_processor.program_counter]) != EMPTY_VALUE)
        {
            my_processor.program_counter++;
            InstructionDecoder(instruction);
        }

        Console.WriteLine("\n\t--------");
        Console.WriteLine("\tRn Value");
        Console.WriteLine("\t--------");
        for (int i = 0; i < REGISTER_COUNT; i++)
        {
            Console.WriteLine($"\tR{i} {my_processor.registers[i],5}");
        }
        Console.WriteLine("\t--------\n");
    }

    static ushort StringToUInt16(string str)
    {
        ushort number = 0;
        foreach (char c in str)
        {
            number = (ushort)(number * 2 + (c - '0'));
        }
        return number;
    }

    static void InstructionDecoder(ushort instruction)
    {
        byte operand3 = (byte)(instruction & 0x07);
        byte operand2 = (byte)((instruction >> 3) & 0x0F);
        byte operand1 = (byte)((instruction >> 7) & 0x07);
        byte i_bit = (byte)((instruction >> 10) & 0x01);
        byte opcode = (byte)((instruction >> 11) & 0x0F);
        byte t_bit = (byte)((instruction >> 15) & 0x01);

        if (t_bit == 1)
        {
            DataTransfer(opcode, operand1, operand2);
        }
        else
        {
            ArithmeticLogicalUnit(opcode, i_bit, operand1, operand2, operand3);
        }
    }

    static void DataTransfer(byte opcode, byte register_index, byte memory_address)
    {
        if (memory_address >= MEMORY_SIZE)
        {
            Console.WriteLine($"Error in {my_processor.program_counter}th Machine Code in {PROGRAM_FILE_PATH} File: Memory Address (Operand 2) exceeds Memory Size {MEMORY_SIZE}");
            my_processor = null;
            Environment.Exit(0);
        }

        if (opcode == LOAD)
        {
            my_processor.registers[register_index] = my_processor.memory[memory_address];
        }
        else if (opcode == STORE)
        {
            my_processor.memory[memory_address] = my_processor.registers[register_index];
        }
        else
        {
            Console.WriteLine($"Error in {my_processor.program_counter}th Machine Code in {PROGRAM_FILE_PATH} File: T-Bit is 1 But OPCode isn't a Data Transfer Operation");
            my_processor = null;
            Environment.Exit(0);
        }
    }

    static void ArithmeticLogicalUnit(byte opcode, byte i_bit, byte instruction_operand1, byte instruction_operand2, byte instruction_operand3)
    {
        ushort operand1;
        ushort operand2 = my_processor.registers[instruction_operand3];

        if (i_bit == 1)
        {
            operand1 = instruction_operand2;
        }
        else
        {
            if (instruction_operand2 > 0b0111)
            {
                Console.WriteLine($"Error in {my_processor.program_counter}th Machine Code in {PROGRAM_FILE_PATH} File: Register Index (Operand 2) is Out Of Bound");
                my_processor = null;
                Environment.Exit(0);
            }

            operand1 = my_processor.registers[instruction_operand2];
        }

        switch (opcode)
        {
            case ADD:
                my_processor.registers[instruction_operand1] = (ushort)(operand1 + operand2);
                break;
            case SUB:
                my_processor.registers[instruction_operand1] = (ushort)(operand1 - operand2);
                break;
            case AND:
                my_processor.registers[instruction_operand1] = (ushort)(operand1 & operand2);
                break;
            case OR:
                my_processor.registers[instruction_operand1] = (ushort)(operand1 | operand2);
                break;
            case XOR:
                my_processor.registers[instruction_operand1] = (ushort)(operand1 ^ operand2);
                break;
            default:
                Console.WriteLine($"Error in {my_processor.program_counter}th Machine Code in {PROGRAM_FILE_PATH} File: T-Bit is 0 But OPCode isn't an ALU Operation");
                my_processor = null;
                Environment.Exit(0);
                break;
        }
    }
}
