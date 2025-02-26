

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <minizip/unzip.h>

namespace fs = std::filesystem;

void extract_logs_from_zip(const std::string &zip_file, const std::string &log_filename, const std::string &date, const std::string &output_dir = "output")
{
    std::string output_file = output_dir + "/output_" + date + ".txt";

    // Ensure output directory exists
    if (!fs::exists(output_dir))
    {
        fs::create_directory(output_dir);
    }

    unzFile zip = unzOpen(zip_file.c_str());
    if (!zip)
    {
        std::cerr << "Error: Could not open ZIP file: " << zip_file << std::endl;
        return;
    }

    // Locate the specific log file inside the ZIP
    if (unzLocateFile(zip, log_filename.c_str(), 0) != UNZ_OK)
    {
        std::cerr << "Error: Could not find " << log_filename << " in ZIP file." << std::endl;
        unzClose(zip);
        return;
    }

    // Open the file inside the ZIP
    if (unzOpenCurrentFile(zip) != UNZ_OK)
    {
        std::cerr << "Error: Could not open file inside ZIP." << std::endl;
        unzClose(zip);
        return;
    }

    std::ofstream outfile(output_file);
    if (!outfile)
    {
        std::cerr << "Error: Could not create output file: " << output_file << std::endl;
        unzClose(zip);
        return;
    }

    char buffer[4096];
    int bytes_read;
    std::string line;
    size_t date_length = date.size();

    while ((bytes_read = unzReadCurrentFile(zip, buffer, sizeof(buffer))) > 0)
    {
        for (int i = 0; i < bytes_read; i++)
        {
            if (buffer[i] == '\n')
            { // End of line
                if (line.rfind(date, 0) == 0)
                { // Check if the line starts with the date
                    outfile << line << "\n";
                }
                line.clear();
            }
            else
            {
                line += buffer[i];
            }
        }
    }

    unzCloseCurrentFile(zip);
    unzClose(zip);
    std::cout << "Extraction complete. Logs saved to: " << output_file << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <YYYY-MM-DD>" << std::endl;
        return 1;
    }

    std::string zip_file = "logs_2024.log.zip"; // Use the provided zip file name
    std::string log_filename = "logs_2024.log"; // File inside the ZIP
    std::string date = argv[1];

    extract_logs_from_zip(zip_file, log_filename, date);

    return 0;
}
