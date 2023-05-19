---

## Running the Docker Image

To run the Docker image and execute the C++ program inside a container, follow the steps below:
1. **Run the progam:**

   ```bash
   ./compare-files file1.ldif file2.ldif output.ldif

2. **Build the Docker image:**

   ```bash
   docker build -t compare-files-image .

Step 2 - Run the Docker container:

3. **Run the Docker container:**

   ```
   docker run --rm -it -v $(pwd):/app/data compare-files:dev ./data/ldif1.ldif ./data/ldif2.ldif ./data/output.ldif 

Step 3 - View the program output:

3. **View the program output:**
   ```
   The program will run inside the Docker container, and the output will be displayed in the terminal or command prompt where you executed the `docker run` command. It will show whether the files are identical or different based on the comparison.

Notes:

Artifacts compile with GLIBC_2.34