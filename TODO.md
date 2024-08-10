# TODO List for Fancy Directory Sort (C Version)
------------------
8/9/2024
-----------------

*Create Package, custom installer that allows you to create default config folders or not, etc. 

1. Implement path length checks and fallback strategies:
   - Add checks to ensure file paths never exceed MAX_PATH limit.
   - Develop fallback strategies for handling exceptionally long paths in production code.

2. Perform comprehensive code cleanup:
   - Remove or comment out unnecessary debug print statements.
   - Ensure consistent code style and formatting throughout the project.

3. Develop a more robust test suite:
   - Expand on the existing Python script that creates random files.
   - Create tests for edge cases and unusual file types.
   - Implement automated testing for different sorting scenarios.

4. Enhance documentation:
   - Add specific examples of how the sorting works with different file types.
   - Include screenshots or diagrams of the program in action.
   - Expand the troubleshooting section with common issues and solutions.

5. Create a website for the project:
   - Design and develop a simple website to showcase the project.
   - Include installation instructions, usage examples, and documentation.
   - Add a download section and links to the GitHub repository.

6. Optimize performance for large directories:
   - Profile the code to identify potential bottlenecks.
   - Implement more efficient algorithms for file sorting if necessary.
   - Consider multi-threading for handling large numbers of files.

7. Implement the uninstall functionality:
   - Add code to remove configuration files and directories created by the program.
   - Ensure all installed components are properly removed during uninstallation.

8. Add support for custom config file locations:
   - Allow users to specify a custom location for config files via command-line option.
   - Update documentation to reflect this new feature.

9. Implement logging functionality:
   - Add proper logging instead of print statements for better debugging and user feedback.
   - Allow users to set log levels (e.g., INFO, DEBUG, ERROR) via command-line options.

10. Consider adding a graphical user interface (GUI):
    - Research appropriate GUI libraries for C (e.g., GTK, Qt).
    - Design a simple interface for the file organizer.
    - Implement basic GUI functionality while maintaining CLI capabilities.
