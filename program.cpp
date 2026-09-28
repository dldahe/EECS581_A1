#include <iostream>
#include <string>
#include <cctype>

// Structure requirements from Canvas:
// Returns true if a valid address was found, false otherwise.
// On success: outAddress holds the 32-bit value,
// and outPort holds the port number, or -1 if no port was present.
// On failure: outAddress is set to 0 and outPort is set to -1.

// extractIPv4 function implementation
bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort)
{
    // initial setup as mentioned in the requirements
    outAddress = 0;
    outPort = -1;

    std::size_t i = 0;

    while (i < str.length())
    {
        // A candidate can only begin with a digit.
        // It cannot begin immediately after another digit, period, or colon
        // that could create apartial match
        // line 20 to 34 was implemented by AI after partial match error
        if (!std::isdigit(static_cast<unsigned char>(str[i])) ||
            (i > 0 &&
             (std::isdigit(static_cast<unsigned char>(str[i - 1])) ||
              str[i - 1] == '.' ||
              str[i - 1] == ':')))
        {
            i++;
            continue;
        }

        std::size_t start = i;
        unsigned int octets[4];
        bool valid = true;

        // Parse exactly four octets.
        for (int octetIndex = 0; octetIndex < 4; octetIndex++)
        {
            if (i >= str.length() ||
                !std::isdigit(static_cast<unsigned char>(str[i])))
            {
                valid = false;
                break;
            }

            int digitCount = 0;
            int value = 0;

            // Parse 1-3 digits.
            while (i < str.length() &&
                   std::isdigit(static_cast<unsigned char>(str[i])))
            {
                if (digitCount == 3)
                {
                    valid = false;
                    break;
                }

                int digit = str[i] - '0';

                // No leading zero unless the value is exactly zero.
                if (digitCount == 0 &&
                    digit == 0 &&
                    i + 1 < str.length() &&
                    std::isdigit(static_cast<unsigned char>(str[i + 1])))
                {
                    valid = false;
                    break;
                }

                // Manually accumulate the numeric value.
                value = value * 10 + digit;
                digitCount++;
                i++;
            }

            if (!valid)
                break;

            // Empty octets and values greater than 255 are invalid.
            if (digitCount == 0 || value > 255)
            {
                valid = false;
                break;
            }

            octets[octetIndex] = value;

            // Every octet except the last must be followed by a period.
            if (octetIndex < 3)
            {
                if (i >= str.length() || str[i] != '.')
                {
                    valid = false;
                    break;
                }

                i++;
            }
        }

        if (!valid)
        {
            // Reject the candidate instead of finding a valid
            // substring inside the malformed candidate.
            i = start + 1;
            continue;
        }

        // Check for an optional port.
        int port = -1;

        if (i < str.length() && str[i] == ':')
        {
            i++;

            int digitCount = 0;
            int portValue = 0;

            // A colon requires a complete valid port.
            if (i >= str.length() ||
                !std::isdigit(static_cast<unsigned char>(str[i])))
            {
                valid = false;
            }
            else
            {
                while (i < str.length() &&
                       std::isdigit(static_cast<unsigned char>(str[i])))
                {
                    if (digitCount == 5)
                    {
                        valid = false;
                        break;
                    }

                    int digit = str[i] - '0';

                    // No leading zero unless the port is exactly zero.
                    if (digitCount == 0 &&
                        digit == 0 &&
                        i + 1 < str.length() &&
                        std::isdigit(static_cast<unsigned char>(str[i + 1])))
                    {
                        valid = false;
                        break;
                    }

                    // Manually accumulate the port value.
                    portValue = portValue * 10 + digit;
                    digitCount++;
                    i++;
                }

                if (digitCount == 0 || portValue > 65535)
                {
                    valid = false;
                }

                if (valid)
                    port = portValue;
            }
        }

        if (!valid)
        {
            i = start + 1;
            continue;
        }

        // A period or colon immediately after a complete address
        // means the candidate is not actually complete.
        if (i < str.length() &&
            (str[i] == '.' || str[i] == ':'))
        {
            i = start + 1;
            continue;
        }

        // Construct the 32-bit decimal representation manually.
        outAddress =
            (static_cast<unsigned long>(octets[0]) << 24) |
            (static_cast<unsigned long>(octets[1]) << 16) |
            (static_cast<unsigned long>(octets[2]) << 8) |
            static_cast<unsigned long>(octets[3]);

        outPort = port;

        return true;
    }

    return false;
}

// main function: Input loop: main continuously prompts the user for input 
// until the user enters END (case-sensitive), then prints Program terminated. and exits.
int main()
{
    std::string input;

    // continous loop until user enters END, prompts user for input
    while (true)
    {
        // Prompt the user for input.
        std::cout << "Enter a string (or 'END' to quit): ";
        std::getline(std::cin, input);

        // check for END : termination condition
        if (input == "END")
        {
            std::cout << "Program terminated." << std::endl;
            break;
        }

        // extract the IPv4 address and port from the input string.
        unsigned long address;
        int port;

        // Call the extractIPv4 function to extract the address and port.
        if (extractIPv4(input, address, port))
        {
            // Extract the dotted-decimal address for display.
            unsigned int a =
                static_cast<unsigned int>((address >> 24) & 255);
            unsigned int b =
                static_cast<unsigned int>((address >> 16) & 255);
            unsigned int c =
                static_cast<unsigned int>((address >> 8) & 255);
            unsigned int d =
                static_cast<unsigned int>(address & 255);

            // display the extracted address and port.
            // follows the sample run format given 
            std::cout << "Extracted IPv4 address: "
                      << a << "." << b << "." << c << "." << d
                      << " (decimal value: " << address
                      << ", port: ";

            if (port == -1)
                std::cout << "none";
            else
                std::cout << port;

            std::cout << ")" << std::endl;
        }
        else
        {
            std::cout << "Invalid input: no valid IPv4 address found"
                      << std::endl;
        }
    }

    return 0;
}