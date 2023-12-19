#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <sstream>

struct Question {
    int id, parent_id, from_user_id, to_user_id;
    std::string question, answer;
    bool anonymous;

    void Print() const {
        if (parent_id != id)
            std::cout << "\t\tThread: ";

        std::cout << "Question id (" << id << ")";
        if (!anonymous)
            std::cout << " from user id(" << from_user_id << ")";
        std::cout << "\t\tQuestion: " << question << "\n";

        if (!answer.empty()) {
            if (parent_id != id)
                std::cout << "\t\tThread: ";
            std::cout << "\t\tAnswer: " << answer << "\n";
        }
        std::cout << "\n";
    }

    void PrintFromUser() const {
        std::cout << "Question id (" << id << ") ";
        if (anonymous)
            std::cout << "AQ ";
        std::cout << "to user id (" << to_user_id << ") ";
        std::cout << "\t\tQuestion " << question << "\n";
        std::cout << "\t\t";
        if (answer.empty())
            std::cout << "NO Answered yet.";
        else
            std::cout << "Answer: " << answer << "\n";
    }

    void PrintFeed() const {
        if (parent_id != id)
            std::cout << "Thread parent question id (" << parent_id << ") , ";

        std::cout << "Question id (" << id << ") ";

        if (!anonymous)
            std::cout << "from user id (" << from_user_id << ") ";
        std::cout << "to user id (" << to_user_id << ") ";

        std::cout << "\t\t" << question << "\n";
        std::cout << "\t\tAnswer: " << answer << "\n\n";
    }
};

struct User {
    int id;
    std::string user_name, full_name, password, Email;
    bool allow_anonymous;

    bool IsEqual(const std::string &other_user_name, const std::string &other_password) const {
        return other_user_name == user_name
               && (other_password == password || other_password.empty());
    }

    void print() const {
        std::cout << "ID: " << id << "\t\tName: " << full_name << "\n";
    }
};

struct Handler {
    std::vector<User> users;
    std::vector<Question> questions;
    std::vector<bool> is_deleted_question;

    Handler() {
        GetUsersData();
        GetQuestionsData();
    }

    ~Handler() {
        SaveUsersData();
        SaveQuestionsData();
    }

    int CheckUserAndPassword(const std::string &user_name, const std::string &password = "") {
        for (auto &user: users) {
            if (user.IsEqual(user_name, password))
                return user.id;
        }
        return -1;
    }

    int CheckQuestionId(int question_id) {
        for (int i = 0; i < questions.size(); i++) {
            if (is_deleted_question[i])continue;
            if (questions[i].id == question_id)
                return question_id;
        }
        return -1;
    }

    int AddUser(User user) {
        if (CheckUserAndPassword(user.user_name) != -1)
            return -1;
        user.id = (int) users.size();
        users.push_back(user);
        return user.id;
    }

    void PrintQuestionsToMe(int user_id) {
        std::unordered_map<int, std::vector<int>> ump;
        for (int i = 0; i < questions.size(); ++i) {
            if (is_deleted_question[i])continue;
            if (questions[i].to_user_id == user_id)
                ump[questions[i].parent_id].push_back(questions[i].id);
        }
        for (const auto &[parent, v]: ump) {
            for (const auto &id: v) {
                questions[id].Print();
            }
        }
    }

    void PrintQuestionsFromMe(int user_id) {
        for (int i = 0; i < questions.size(); ++i) {
            if (is_deleted_question[i])continue;
            if (questions[i].from_user_id == user_id)
                questions[i].PrintFromUser();
        }
    }

    std::pair<int, bool> get_user(int user_id) {
        if (user_id >= 0 && user_id < users.size() && users[user_id].id == user_id) {
            return {user_id, users[user_id].allow_anonymous};
        }
        return {-1, false};
    }

    int IsQuestionToUser(int question_id, int user_id) {
        //invalid question id
        if (question_id >= questions.size() || question_id < 0)
            return 0;
        if (is_deleted_question[question_id])
            return 0;
        //has no right to answer the question
        if (questions[question_id].to_user_id != user_id)
            return 1;

        //print
        questions[question_id].Print();

        //say that has answer
        if (questions[question_id].answer.empty() == 0)
            return 2;
        return 3;
    }

    void AnswerQuestion(int qustion_id, std::string &answer_text) {
        questions[qustion_id].answer = answer_text;
    }

    bool DeleteQuestion(int question_id, int user_id) {
        for (int i = 0; i < questions.size(); ++i) {
            if (is_deleted_question[i]) continue;
            if (questions[i].id == question_id) {

                if (questions[i].from_user_id != user_id)//can't delete this question
                    return false;
            }
            if (questions[i].parent_id == question_id) {
                is_deleted_question[i] = true;
            }
        }
        return true;
    }

    void AskQuestion(int to, int from, int parent, std::string &question_text, bool anonymous) {
        int id = (int) questions.size();
        if (parent == -1)
            parent = id;
        Question new_question = {id, parent, from, to,
                                 question_text, "", anonymous};
        questions.push_back(new_question);
        is_deleted_question.push_back(false);
    }

    void ListSystemUsers() {
        for (auto &user: users)
            user.print();

    }

    void Feed() {
        for (auto &question: questions)
            question.PrintFeed();
    }

    void GetUsersData() {
        std::ifstream user_file("users.txt");
        if (!user_file.is_open()) {
            std::cout << "Error: Can't Open 'users.txt'\n";
            return;
        }
        std::string input;
        while (getline(user_file, input)) {
            std::stringstream line(input);
            User user;
            line >> user.id >> user.user_name >> user.password >> user.allow_anonymous;

            getline(user_file, user.full_name);

            users.push_back(user);
        }
        user_file.close();
    }

    void GetQuestionsData() {
        std::ifstream questions_file("questions.txt");
        if (!questions_file.is_open()) {
            std::cout << "Error: Can't Open 'questions.txt'\n";
            return;
        }
        std::string input;
        while (getline(questions_file, input)) {
            std::stringstream line(input);
            Question q;
            bool is_deleted;
            line >> q.id >> q.parent_id >> q.from_user_id >>
                 q.to_user_id >> q.anonymous >> is_deleted;

            getline(questions_file, q.question);
            getline(questions_file, q.answer);

            questions.push_back(q);
            is_deleted_question.push_back(is_deleted);
        }
        questions_file.close();
    }

    void SaveUsersData() {
        std::ofstream user_file("users.txt");
        if (!user_file.is_open()) {
            std::cout << "Error: Can't Open 'users.txt'\n";
            return;
        }
        for (auto &user: users) {
            user_file << user.id << " " << user.user_name << " " << user.password << " " <<
                      user.allow_anonymous << "\n";
            user_file << user.full_name << "\n";
        }
    }

    void SaveQuestionsData() {
        std::ofstream questions_file("questions.txt");
        if (!questions_file.is_open()) {
            std::cout << "Error: Can't Open 'questions.txt'\n";
            return;
        }
        for (int i = 0; i < questions.size(); i++) {
            auto q = questions[i];
            bool is_deleted = is_deleted_question[i];
            questions_file << q.id << " " << q.parent_id << " " << q.from_user_id << " "
                           << q.to_user_id << " " << q.anonymous << " " << is_deleted << "\n";
            questions_file << q.question << "\n";
            questions_file << q.answer << "\n";
        }
    }
};

struct AskMeSystem {
    Handler handler;

    void run() {
        bool running = true;
        while (running) {
            int user_id = MainMenu();
            running = UserMenu(user_id);
        }
    }

    int MainMenu() {
        std::cout << "Menu:\n";
        std::cout << "\t\t1: Login\n";
        std::cout << "\t\t2: Sign UP\n";
        int id = -1;
        while (id == -1) {
            std::cout << "Enter number in range 1 - 2: ";
            int t;
            std::cin >> t;
            if (t == 1)
                id = Login();
            else if (t == 2)
                id = SignUp();
            else
                std::cout << "Invalid choice.";
        }
        return id;
    }

    bool UserMenu(int user_id) {
        std::cout << "Menu:\n";
        std::cout << "\t\t1: Print Questions To Me.\n";
        std::cout << "\t\t2: Print Questions From Me.\n";
        std::cout << "\t\t3: Answer Question.\n";
        std::cout << "\t\t4: Delete Question.\n";
        std::cout << "\t\t5: Ask Question.\n";
        std::cout << "\t\t6: List System Users.\n";
        std::cout << "\t\t7: Feed.\n";
        std::cout << "\t\t8: Logout.\n";
        std::cout << "\t\t9: Close Program.\n";

        int value;
        while (true) {
            std::cout << "\nEnter number in range 1 - 9: ";
            std::cin >> value;

            if (value == 1) {
                handler.PrintQuestionsToMe(user_id);

            } else if (value == 2) {
                handler.PrintQuestionsFromMe(user_id);

            } else if (value == 3) {
                int question_id = GetQuestionId(false);
                if (question_id != -1) {

                    int status = handler.IsQuestionToUser(question_id, user_id);

                    if (status == 0) {
                        std::cout << "Invalid question id.\n";
                        continue;
                    } else if (status == 1) {
                        std::cout << "Not allowed to answer this question.\n";
                        continue;
                    } else if (status == 3)
                        std::cout << "Note: This question has been already answered.\n";
                    std::cout << "Enter your answer: ";
                    std::string answer_text;
                    std::cin.ignore();
                    getline(std::cin, answer_text);

                    handler.AnswerQuestion(question_id, answer_text);
                }

            } else if (value == 4) {
                int question_id = GetQuestionId(false);

                if (question_id != -1) {
                    if (handler.DeleteQuestion(question_id, user_id))
                        std::cout << "Successfully deleted.\n";
                    else
                        std::cout << "You aren\'t allowed to delete this question.\n";
                }

            } else if (value == 5) {
                int to, question_id, from = user_id;
                std::string question_text;
                std::cout << "Enter user id or -1 for cancel: ";
                std::cin >> to;

                if (to != -1) {
                    bool anonymous = AskQuestion(to, question_id, question_text);
                    if (to == -1) {
                        std::cout << "Invalid user id\n";
                    } else
                        handler.AskQuestion(to, from, question_id, question_text, anonymous);
                }
            } else if (value == 6) {
                handler.ListSystemUsers();
            } else if (value == 7) {
                handler.Feed();
            } else if (value == 8) {
                std::cout << "Logging out...\n";
                value = 1;
                break;
            } else if (value == 9) {
                std::cout << "Ending Program...\n";
                value = 0;
                break;
            }
        }
        return bool(value);
    }

    bool AskQuestion(int &to, int &question_id, std::string &question_text) {
        bool allow_anonymous;
        std::tie(to, allow_anonymous) = handler.get_user(to);
        if (to == -1) {
            std::cout << "Invalid user id.\n";
        } else {
            if (allow_anonymous) {
                int val;
                std::cout << "Do you wanna ask in anonymous mode (0 for no, otherwise for yes): ";
                std::cin >> val;
                allow_anonymous = val;
            } else {
                std::cout << "Anonymous questions are not allowed for this user.\n";
            }
            question_id = GetQuestionId(true);

            std::cout << "Enter question text: ";
            std::cin.ignore();
            getline(std::cin, question_text);
        }
        return allow_anonymous;
    }

    int GetQuestionId(bool thread) {
        int question_id;

        if (thread)
            std::cout << "For thread question: Enter question id or -1 for new question: ";
        else
            std::cout << "Enter Question id or -1 to cancel: ";

        std::cin >> question_id;
        if (question_id != -1) {
            question_id = handler.CheckQuestionId(question_id);

            if (question_id == -1) {
                std::cout << "Invalid question id.\n";

                if (thread)
                    std::cout << "We will add new questions.\n";
            }
        }
        return question_id;
    }

    int Login() {
        std::string user_name, password;
        std::cout << "Enter user name. (no spaces): ";
        std::cin >> user_name;
        std::cout << "Enter password: ";
        std::cin >> password;
        int user_id = handler.CheckUserAndPassword(user_name, password);
        if (user_id == -1)
            std::cout << "Invalid sign in, wrong with user name or password.\n";
        return user_id;
    }

    int SignUp() {
        std::string user_name, full_name, password, email;
        int allow_anonymous;
        std::cout << "Enter user name. (no spaces):";
        std::cin >> user_name;
        std::cout << "Enter password: ";
        std::cin >> password;
        std::cout << "Enter name: ";
        std::cin.ignore();
        getline(std::cin, full_name);
        std::cout << "Enter email: ";
        std::cin >> email;
        std::cout << "Allow anonymous question? (0 for no,otherwise for yes): ";
        std::cin >> allow_anonymous;

        int user_id = handler.AddUser({-1, user_name, full_name,
                                       password, email, (bool) allow_anonymous});
        if (user_id == -1) {
            std::cout << "Invalid sign up, try another user name.\n";
        }
        return user_id;
    }
};

int main() {
    AskMeSystem Project;
    Project.run();

    return 0;
}
