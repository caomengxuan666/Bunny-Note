Markdown Editor
A simple and efficient Markdown editor built with C++ and Qt. This application provides a seamless writing experience with real-time preview, syntax highlighting, and customizable themes.

Features
Real-time Preview: Automatically updates the preview pane as you type.
Syntax Highlighting: Supports code block highlighting using Highlight.js.
Multiple Themes: Choose from Light, Dark, Solarized Light, and Solarized Dark themes.
File Management:
Open and edit existing Markdown files.
Create new files within the application.
Delete unwanted files easily.
Tabbed Interface: Work on multiple documents simultaneously with tab support.
Customizable Editor:
Change font type and size.
Set tab width to match your coding style.
Insert Images: Quickly insert images into your Markdown documents.
Auto-save: Automatically saves your work every 10 seconds to prevent data loss.
Split View Adjustments: Resize editor and preview panes with customized splitter handles for better user experience.
Screenshots

An example of the editor with real-time preview and syntax highlighting.

Getting Started
Prerequisites
Qt Framework: Ensure you have Qt installed (version 5.12 or later recommended).
C++ Compiler: A C++17 compatible compiler (e.g., GCC, Clang, MSVC).
Clone the Repository
bash
复制代码
git clone https://github.com/yourusername/markdown-editor.git
cd markdown-editor
Build Instructions
Open the Project in Qt Creator:

Launch Qt Creator.
Click on "Open Project" and select the markdown-editor.pro file.
Configure the Project:

Choose the appropriate kit for your development environment.
Ensure that the compiler and Qt version are correctly set.
Build and Run:

Click on the "Run" button to build and launch the application.
Running from the Command Line
bash
复制代码
# Navigate to the project directory
cd markdown-editor

# Create a build directory
mkdir build && cd build

# Generate Makefiles using qmake
qmake ..

# Build the project
make

# Run the application
./markdown-editor
Usage
Opening Files:
Use the file list on the left to open existing Markdown files in the current directory.
You can also open files from other directories using File > Open File.
Creating New Files:
Go to File > New File or press Ctrl+N.
Enter the desired filename (without extension).
Saving Files:
Save your work with File > Save File or press Ctrl+S.
Use File > Save File As to save the current document under a new name.
Deleting Files:
Select a file from the list and choose File > Delete File or press Ctrl+D.
Inserting Images:
Place the cursor where you want to insert the image.
Go to File > Insert Image or press Ctrl+I.
Select an image file to insert.
Changing Themes:
Select your preferred theme from the Themes menu.
Adjusting Font:
Choose File > Set Font or press Ctrl+P to change the editor's font and size.
Customization
Themes
The application supports four themes:

* Light
* Dark
* Solarized Light
* Solarized Dark
You can add more themes by modifying the updatePalette and applyThemeToAllTabs functions in MainWindow.cpp.

Syntax Highlighting Styles
The syntax highlighting uses Highlight.js with the GitHub style. To change the style:

Download the desired CSS file from the Highlight.js styles.
Update the highlightCss variable in the loadMarkdown function with the new CSS file path.
Project Structure
main.cpp: Entry point of the application.
MainWindow.h / MainWindow.cpp: Contains the main window logic, UI setup, and event handling.
HtmlConverter.hpp: Utility class for converting Markdown to HTML.
settings.h: Manages application settings like theme, font, and last opened file.
resources/: Contains images and other resource files.
screenshots/: Holds images used in the README.
Dependencies
Qt: For the GUI framework and web engine components.
Highlight.js: For syntax highlighting in code blocks.
cmark: A library for parsing Markdown to HTML.
Contributing
Contributions are welcome! Please follow these steps:

Fork the repository.
Create a new branch: git checkout -b feature/your-feature.
Commit your changes: git commit -am 'Add some feature'.
Push to the branch: git push origin feature/your-feature.
Create a pull request.
License
This project is licensed under the MIT License. See the LICENSE file for details.

Acknowledgments
Qt Documentation: For comprehensive guides and examples.
Highlight.js: For providing an easy way to add syntax highlighting.
cmark: For the Markdown parsing library.
Contact
For questions or suggestions, please open an issue or contact your email.
