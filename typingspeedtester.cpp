#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <chrono>
#include <thread>
using namespace std;

struct TypingStats {
    string name;
    double wpm;
    double accuracy;
    double time;
    int correctChars;
    int totalChars;
    int errors;
    string difficulty;
    string date;
};

class TypingSpeedChecker {
private:
    vector<TypingStats> history;
    vector<TypingStats> leaderboard;
    string currentUser;
    
    vector<string> easyTexts = {
        "The cat sat on the mat.",
        "I love to code every day.",
        "Practice makes perfect."
    };
    
    vector<string> mediumTexts = {
        "The quick brown fox jumps over the lazy dog near the riverbank.",
        "Programming is both an art and a science that requires practice.",
        "Efficient typing skills improve productivity and save valuable time."
    };
    
    vector<string> hardTexts = {
        "In computer science, algorithms and data structures form the foundation of efficient problem-solving techniques.",
        "The implementation of object-oriented programming principles enhances code reusability and maintainability.",
        "Cybersecurity professionals must stay vigilant against evolving threats in our interconnected digital landscape."
    };
    
    vector<string> expertTexts = {
        "The polymorphic nature of C++ enables developers to write flexible, extensible code through inheritance and virtual functions.",
        "Asynchronous programming paradigms facilitate non-blocking I/O operations, significantly improving application responsiveness.",
        "Quantum computing's superposition principle allows qubits to exist in multiple states simultaneously, exponentially increasing computational power."
    };

    string getCurrentDate() {
        time_t now = time(0);
        char buffer[80];
        struct tm* timeinfo = localtime(&now);
        strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeinfo);
        return string(buffer);
    }

public:
    TypingSpeedChecker() {
        loadHistory();
        loadLeaderboard();
    }
    
    ~TypingSpeedChecker() {
        saveHistory();
        saveLeaderboard();
    }
    
    void setUser(const string& name) {
        currentUser = name;
    }
    
    double calculateWPM(int correctChars, double timeInSeconds) {
        // Strict validation for macOS
        if (timeInSeconds < 0.5) {
            cout << "⚠️ Warning: Timing error detected (" << timeInSeconds << "s)\n";
            return 0;
        }
        
        double minutes = timeInSeconds / 60.0;
        double words = correctChars / 5.0;
        double wpm = words / minutes;
        
        // Cap at 300 WPM (world record territory)
        if (wpm > 300) {
            cout << "⚠️ Warning: Unrealistic WPM detected, timing may be incorrect\n";
            return 0;
        }
        
        return wpm;
    }
    
    double calculateAccuracy(const string& original, const string& typed) {
        int correct = 0;
        int total = max(original.length(), typed.length());
        
        for (size_t i = 0; i < min(original.length(), typed.length()); i++) {
            if (original[i] == typed[i]) {
                correct++;
            }
        }
        
        return total > 0 ? (double)correct / total * 100.0 : 0;
    }
    
    int countErrors(const string& original, const string& typed) {
        int errors = 0;
        size_t maxLen = max(original.length(), typed.length());
        
        for (size_t i = 0; i < maxLen; i++) {
            if (i >= original.length() || i >= typed.length() || original[i] != typed[i]) {
                errors++;
            }
        }
        
        return errors;
    }
    
    void displayDetailedComparison(const string& original, const string& typed) {
        cout << "\n--- Character-by-Character Analysis ---\n";
        cout << "Original: " << original << "\n";
        cout << "Typed:    " << typed << "\n";
        cout << "Match:    ";
        
        for (size_t i = 0; i < max(original.length(), typed.length()); i++) {
            if (i < original.length() && i < typed.length() && original[i] == typed[i]) {
                cout << "✓";
            } else {
                cout << "✗";
            }
        }
        cout << "\n";
    }
    
    void displayRealTimeHighlight(const string& original, const string& typed) {
        cout << "\n--- Real-Time Character Highlighting ---\n";
        cout << "Legend: [Correct] (Wrong) _Missing_\n\n";
        
        for (size_t i = 0; i < original.length(); i++) {
            if (i < typed.length()) {
                if (original[i] == typed[i]) {
                    cout << "\033[32m[" << original[i] << "]\033[0m";
                } else {
                    cout << "\033[31m(" << typed[i] << ")\033[0m";
                }
            } else {
                cout << "\033[33m_" << original[i] << "_\033[0m";
            }
        }
        
        if (typed.length() > original.length()) {
            cout << "\033[31m";
            for (size_t i = original.length(); i < typed.length(); i++) {
                cout << "+" << typed[i];
            }
            cout << "\033[0m";
        }
        cout << "\n";
    }
    
    string selectText(int difficulty) {
        srand(time(0));
        vector<string>* texts;
        
        switch(difficulty) {
            case 1: texts = &easyTexts; break;
            case 2: texts = &mediumTexts; break;
            case 3: texts = &hardTexts; break;
            case 4: texts = &expertTexts; break;
            default: texts = &mediumTexts;
        }
        
        return (*texts)[rand() % texts->size()];
    }
    
    void timedMode() {
        cout << "\n===========================================\n";
        cout << "      ⏱️  60-SECOND TIMED MODE ⏱️\n";
        cout << "===========================================\n";
        cout << "Type as much as you can in 60 seconds!\n";
        cout << "The text will repeat if you finish early.\n\n";
        
        string practiceText = "The quick brown fox jumps over the lazy dog. ";
        practiceText += "Practice makes perfect. ";
        practiceText += "Keep typing to improve your speed. ";
        practiceText += "Focus on accuracy and speed together. ";
        
        string fullText = "";
        for (int i = 0; i < 20; i++) {
            fullText += practiceText;
        }
        
        cout << "Text to type (repeating):\n";
        cout << "\"" << practiceText << "\"\n\n";
        cout << "Press ENTER to start the 60-second timer...\n";
        cin.ignore();
        
        cout << "\n⏱️  START TYPING NOW! You have 60 seconds! ⏱️\n\n";
        
        auto startTime = chrono::high_resolution_clock::now();
        auto endTime = startTime + chrono::seconds(60);
        
        string typedText = "";
        cout << "Type here: ";
        
        while (chrono::high_resolution_clock::now() < endTime) {
            if (cin.peek() != EOF) {
                getline(cin, typedText);
                break;
            }
        }
        
        auto actualEndTime = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = actualEndTime - startTime;
        double timeTaken = min(elapsed.count(), 60.0);
        
        string textToCompare = fullText.substr(0, typedText.length());
        double accuracy = calculateAccuracy(textToCompare, typedText);
        int correctChars = 0;
        
        for (size_t i = 0; i < min(textToCompare.length(), typedText.length()); i++) {
            if (textToCompare[i] == typedText[i]) {
                correctChars++;
            }
        }
        
        double wpm = calculateWPM(correctChars, timeTaken);
        int errors = countErrors(textToCompare, typedText);
        
        TypingStats stats;
        stats.name = currentUser;
        stats.wpm = wpm;
        stats.accuracy = accuracy;
        stats.time = timeTaken;
        stats.correctChars = correctChars;
        stats.totalChars = typedText.length();
        stats.errors = errors;
        stats.difficulty = "Timed-60s";
        stats.date = getCurrentDate();
        
        history.push_back(stats);
        updateLeaderboard(stats);
        
        displayResults(stats);
        displayPerformanceAnalysis(stats);
    }
    
    void customTextMode() {
        cout << "\n===========================================\n";
        cout << "      📝 CUSTOM TEXT MODE 📝\n";
        cout << "===========================================\n";
        cout << "Enter your custom text to practice:\n";
        cout << "(Press ENTER twice when done)\n\n";
        
        cin.ignore();
        string customText;
        string line;
        
        while (getline(cin, line) && !line.empty()) {
            customText += line + " ";
        }
        
        if (customText.empty()) {
            cout << "❌ No text entered. Returning to menu.\n";
            return;
        }
        
        cout << "\nYour custom text:\n\"" << customText << "\"\n\n";
        cout << "Press ENTER when ready to start...\n";
        cin.get();
        
        cout << "\n⏱️  START TYPING NOW! ⏱️\n\n";
        
        auto startTime = chrono::high_resolution_clock::now();
        string typedText;
        getline(cin, typedText);
        auto endTime = chrono::high_resolution_clock::now();
        
        chrono::duration<double> elapsed = endTime - startTime;
        double timeTaken = elapsed.count();
        
        double accuracy = calculateAccuracy(customText, typedText);
        int correctChars = 0;
        
        for (size_t i = 0; i < min(customText.length(), typedText.length()); i++) {
            if (customText[i] == typedText[i]) {
                correctChars++;
            }
        }
        
        double wpm = calculateWPM(correctChars, timeTaken);
        int errors = countErrors(customText, typedText);
        
        TypingStats stats;
        stats.name = currentUser;
        stats.wpm = wpm;
        stats.accuracy = accuracy;
        stats.time = timeTaken;
        stats.correctChars = correctChars;
        stats.totalChars = typedText.length();
        stats.errors = errors;
        stats.difficulty = "Custom";
        stats.date = getCurrentDate();
        
        history.push_back(stats);
        updateLeaderboard(stats);
        
        displayResults(stats);
        displayRealTimeHighlight(customText, typedText);
        displayPerformanceAnalysis(stats);
    }
    
    void startTest(int difficulty, string& difficultyLevel) {
        string originalText = selectText(difficulty);
        
        switch(difficulty) {
            case 1: difficultyLevel = "Easy"; break;
            case 2: difficultyLevel = "Medium"; break;
            case 3: difficultyLevel = "Hard"; break;
            case 4: difficultyLevel = "Expert"; break;
        }
        
        cout << "\n===========================================\n";
        cout << "  Difficulty: " << difficultyLevel << "\n";
        cout << "===========================================\n\n";
        cout << "Type the following text:\n\n";
        cout << "\"" << originalText << "\"\n\n";
        cout << "Character count: " << originalText.length() << "\n";
        cout << "\nPress ENTER when ready to start...\n";
        cin.ignore();
        
        cout << "\n⏱️  START TYPING NOW! ⏱️\n\n";
        
        auto startTime = chrono::high_resolution_clock::now();
        string typedText;
        getline(cin, typedText);
        auto endTime = chrono::high_resolution_clock::now();
        
        chrono::duration<double> elapsed = endTime - startTime;
        double timeTaken = elapsed.count();
        
        cout << "\n⏱️ Time measured: " << fixed << setprecision(2) << timeTaken << " seconds\n";
        
        double accuracy = calculateAccuracy(originalText, typedText);
        int correctChars = 0;
        
        for (size_t i = 0; i < min(originalText.length(), typedText.length()); i++) {
            if (originalText[i] == typedText[i]) {
                correctChars++;
            }
        }
        
        double wpm = calculateWPM(correctChars, timeTaken);
        int errors = countErrors(originalText, typedText);
        
        TypingStats stats;
        stats.name = currentUser;
        stats.wpm = wpm;
        stats.accuracy = accuracy;
        stats.time = timeTaken;
        stats.correctChars = correctChars;
        stats.totalChars = typedText.length();
        stats.errors = errors;
        stats.difficulty = difficultyLevel;
        stats.date = getCurrentDate();
        
        history.push_back(stats);
        updateLeaderboard(stats);
        
        displayResults(stats);
        displayRealTimeHighlight(originalText, typedText);
        displayDetailedComparison(originalText, typedText);
        displayPerformanceAnalysis(stats);
    }
    
    void displayResults(const TypingStats& stats) {
        cout << "\n===========================================\n";
        cout << "              📊 RESULTS 📊\n";
        cout << "===========================================\n";
        cout << fixed << setprecision(2);
        cout << "👤 Typist:            " << stats.name << "\n";
        cout << "⏱️  Time Taken:        " << stats.time << " seconds\n";
        cout << "🎯 Accuracy:          " << stats.accuracy << "%\n";
        cout << "⚡ Typing Speed:      " << stats.wpm << " WPM\n";
        cout << "✅ Correct Characters: " << stats.correctChars << "\n";
        cout << "📝 Total Typed:       " << stats.totalChars << "\n";
        cout << "❌ Errors:            " << stats.errors << "\n";
        cout << "📊 Difficulty:        " << stats.difficulty << "\n";
        cout << "📅 Date:              " << stats.date << "\n";
        cout << "===========================================\n";
    }
    
    void displayPerformanceAnalysis(const TypingStats& stats) {
        cout << "\n--- Performance Analysis ---\n";
        
        cout << "Speed Rating: ";
        if (stats.wpm >= 80) cout << "⭐⭐⭐⭐⭐ Elite\n";
        else if (stats.wpm >= 60) cout << "⭐⭐⭐⭐ Excellent\n";
        else if (stats.wpm >= 40) cout << "⭐⭐⭐ Good\n";
        else if (stats.wpm >= 25) cout << "⭐⭐ Average\n";
        else if (stats.wpm > 0) cout << "⭐ Beginner\n";
        else cout << "❌ Invalid timing\n";
        
        cout << "Accuracy Rating: ";
        if (stats.accuracy >= 98) cout << "⭐⭐⭐⭐⭐ Perfect\n";
        else if (stats.accuracy >= 95) cout << "⭐⭐⭐⭐ Excellent\n";
        else if (stats.accuracy >= 90) cout << "⭐⭐⭐ Good\n";
        else if (stats.accuracy >= 80) cout << "⭐⭐ Fair\n";
        else cout << "⭐ Needs Practice\n";
        
        if (stats.wpm > 0) {
            cout << "\n💡 Feedback:\n";
            if (stats.wpm >= 60 && stats.accuracy >= 95) {
                cout << "   Outstanding! You're in the top tier of typists.\n";
            } else if (stats.wpm >= 40 && stats.accuracy >= 85) {
                cout << "   Great work! Keep practicing to reach expert level.\n";
            } else if (stats.accuracy < 80) {
                cout << "   Focus on accuracy before speed. Slow down and reduce errors.\n";
            } else if (stats.wpm < 40) {
                cout << "   Practice regularly to build muscle memory and speed.\n";
            }
        }
    }
    
    void viewHistory() {
        if (history.empty()) {
            cout << "\n❌ No typing history available yet!\n";
            return;
        }
        
        cout << "\n===========================================\n";
        cout << "         📈 TYPING HISTORY 📈\n";
        cout << "===========================================\n\n";
        
        double avgWPM = 0, avgAccuracy = 0;
        int validTests = 0;
        
        for (size_t i = 0; i < history.size(); i++) {
            cout << "Test #" << (i + 1) << " - " << history[i].name << " [" << history[i].difficulty << "]\n";
            cout << "  Speed: " << fixed << setprecision(2) << history[i].wpm << " WPM | ";
            cout << "Accuracy: " << history[i].accuracy << "% | ";
            cout << "Time: " << history[i].time << "s\n";
            cout << "  Date: " << history[i].date << "\n\n";
            
            if (history[i].wpm > 0) {
                avgWPM += history[i].wpm;
                avgAccuracy += history[i].accuracy;
                validTests++;
            }
        }
        
        if (validTests > 0) {
            avgWPM /= validTests;
            avgAccuracy /= validTests;
            
            cout << "--- Overall Statistics ---\n";
            cout << "Total Tests: " << history.size() << " (Valid: " << validTests << ")\n";
            cout << "Average Speed: " << avgWPM << " WPM\n";
            cout << "Average Accuracy: " << avgAccuracy << "%\n";
        }
        cout << "===========================================\n";
    }
    
    void updateLeaderboard(const TypingStats& stats) {
        if (stats.wpm <= 0) return; // Don't add invalid entries
        
        leaderboard.push_back(stats);
        
        sort(leaderboard.begin(), leaderboard.end(), [](const TypingStats& a, const TypingStats& b) {
            if (abs(a.wpm - b.wpm) < 0.01) {
                return a.accuracy > b.accuracy;
            }
            return a.wpm > b.wpm;
        });
        
        if (leaderboard.size() > 10) {
            leaderboard.resize(10);
        }
    }
    
    void displayLeaderboard() {
        if (leaderboard.empty()) {
            cout << "\n❌ Leaderboard is empty!\n";
            return;
        }
        
        cout << "\n===========================================\n";
        cout << "         🏆 LEADERBOARD 🏆\n";
        cout << "===========================================\n\n";
        
        cout << left << setw(5) << "Rank" << setw(15) << "Name" 
             << setw(10) << "WPM" << setw(12) << "Accuracy" 
             << setw(12) << "Difficulty" << "\n";
        cout << string(54, '-') << "\n";
        
        for (size_t i = 0; i < leaderboard.size(); i++) {
            string medal = "";
            if (i == 0) medal = "🥇 ";
            else if (i == 1) medal = "🥈 ";
            else if (i == 2) medal = "🥉 ";
            else medal = to_string(i + 1) + ".  ";
            
            cout << left << setw(5) << medal
                 << setw(15) << leaderboard[i].name
                 << setw(10) << fixed << setprecision(2) << leaderboard[i].wpm
                 << setw(12) << leaderboard[i].accuracy << "%"
                 << setw(12) << leaderboard[i].difficulty << "\n";
        }
        
        cout << "===========================================\n";
    }
    
    void saveHistory() {
        ofstream file("typing_history.txt");
        if (!file.is_open()) return;
        
        for (const auto& stat : history) {
            file << stat.name << "|" << stat.wpm << "|" << stat.accuracy << "|"
                 << stat.time << "|" << stat.correctChars << "|" << stat.totalChars << "|"
                 << stat.errors << "|" << stat.difficulty << "|" << stat.date << "\n";
        }
        
        file.close();
    }
    
    void loadHistory() {
        ifstream file("typing_history.txt");
        if (!file.is_open()) return;
        
        string line;
        while (getline(file, line)) {
            TypingStats stat;
            stringstream ss(line);
            string token;
            
            getline(ss, stat.name, '|');
            getline(ss, token, '|'); stat.wpm = stod(token);
            getline(ss, token, '|'); stat.accuracy = stod(token);
            getline(ss, token, '|'); stat.time = stod(token);
            getline(ss, token, '|'); stat.correctChars = stoi(token);
            getline(ss, token, '|'); stat.totalChars = stoi(token);
            getline(ss, token, '|'); stat.errors = stoi(token);
            getline(ss, stat.difficulty, '|');
            getline(ss, stat.date);
            
            history.push_back(stat);
        }
        
        file.close();
    }
    
    void saveLeaderboard() {
        ofstream file("leaderboard.txt");
        if (!file.is_open()) return;
        
        for (const auto& stat : leaderboard) {
            file << stat.name << "|" << stat.wpm << "|" << stat.accuracy << "|"
                 << stat.difficulty << "|" << stat.date << "\n";
        }
        
        file.close();
    }
    
    void loadLeaderboard() {
        ifstream file("leaderboard.txt");
        if (!file.is_open()) return;
        
        string line;
        while (getline(file, line)) {
            TypingStats stat;
            stringstream ss(line);
            string token;
            
            getline(ss, stat.name, '|');
            getline(ss, token, '|'); stat.wpm = stod(token);
            getline(ss, token, '|'); stat.accuracy = stod(token);
            getline(ss, stat.difficulty, '|');
            getline(ss, stat.date);
            
            leaderboard.push_back(stat);
        }
        
        file.close();
    }
    
    void displayMenu() {
        cout << "\n===========================================\n";
        cout << "    ⌨️  TYPING SPEED CHECKER PRO ⌨️\n";
        cout << "===========================================\n";
        cout << "1. 🟢 Easy Mode\n";
        cout << "2. 🟡 Medium Mode\n";
        cout << "3. 🟠 Hard Mode\n";
        cout << "4. 🔴 Expert Mode\n";
        cout << "5. ⏱️  60-Second Timed Mode\n";
        cout << "6. 📝 Custom Text Mode\n";
        cout << "7. 📈 View History\n";
        cout << "8. 🏆 View Leaderboard\n";
        cout << "9. 🚪 Exit\n";
        cout << "===========================================\n";
        cout << "Enter your choice: ";
    }
};

int main() {
    TypingSpeedChecker checker;
    int choice;
    string difficultyLevel;
    string userName;
    
    cout << "\n===========================================\n";
    cout << "    ⌨️  TYPING SPEED CHECKER PRO ⌨️\n";
    cout << "===========================================\n";
    cout << "👤 Enter your name: ";
    getline(cin, userName);
    
    if (userName.empty()) {
        userName = "Guest";
    }
    
    checker.setUser(userName);
    cout << "\nWelcome, " << userName << "! 🎉\n";
    
    while (true) {
        checker.displayMenu();
        cin >> choice;
        
        if (choice == 9) {
            cout << "\n👋 Thanks for practicing, " << userName << "! Keep improving!\n";
            cout << "Your progress has been saved. 💾\n";
            break;
        }
        
        if (choice >= 1 && choice <= 4) {
            checker.startTest(choice, difficultyLevel);
            cout << "\n🔄 Press ENTER to continue...\n";
            cin.ignore();
            cin.get();
        } else if (choice == 5) {
            checker.timedMode();
            cout << "\n🔄 Press ENTER to continue...\n";
            cin.ignore();
            cin.get();
        } else if (choice == 6) {
            checker.customTextMode();
            cout << "\n🔄 Press ENTER to continue...\n";
            cin.get();
        } else if (choice == 7) {
            checker.viewHistory();
            cout << "\n🔄 Press ENTER to continue...\n";
            cin.ignore();
            cin.get();
        } else if (choice == 8) {
            checker.displayLeaderboard();
            cout << "\n🔄 Press ENTER to continue...\n";
            cin.ignore();
            cin.get();
        } else {
            cout << "\n❌ Invalid choice! Please try again.\n";
            cin.ignore();
        }
    }
    
    return 0;
}