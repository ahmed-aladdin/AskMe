# AskMe System

It's a simple console-based Q&A platform implemented in C++ and it's a file-based system.
The system allows for user registration, login, asking, answering, and deleting questions.
Users can ask threaded questions.

## Features

- **User Authentication:**
   - Users can sign up with a unique username and password.
   - Existing users can log in to the system.

- **Asking and Answering Questions:**
   - Users can ask questions and provide answers.
   - Questions can be organized into threads.

- **Threaded Questions:**
   - Questions and answers are organized in a threaded format.

- **Anonymous Questions:**
   - Users can choose to ask questions anonymously.

- **Question Feed:**
   - Users can view a feed of all questions and answers in the system.
- **Data Persistence:**
   - User and question data are saved to files for persistence between program executions.
     
## System behavior 

The system is based on two main components:
- **Main System**
  - Interact with users
  - Give data to handler
  - Ask **handler** for specific data
- **Handler**
  - Manages users and questions.
  - Handles user authentication, question management, and data persistence.
  - Providing results to the **Main system**
