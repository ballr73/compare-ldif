---

## Running the Docker Image

To run the Docker image and execute the C++ program inside a container, follow the steps below:

1. **Build the Docker image:**

   ```bash
   docker build -t compare-files-image .

Step 2 - Run the Docker container:

```markdown
2. **Run the Docker container:**

   ```bash
   docker run --rm -it compare-files-image [arguments]
   docker run --rm -it -v /path/to/compare/files:/app/compare_files compare-files-image [arguments]

   docker run --rm -it -v /home/user/compare_files:/app/compare_files compare-files-image file1.txt file2.txt


Step 3 - View the program output:

```markdown
3. **View the program output:**

   The program will run inside the Docker container, and the output will be displayed in the terminal or command prompt where you executed the `docker run` command. It will show whether the files are identical or different based on the comparison.
