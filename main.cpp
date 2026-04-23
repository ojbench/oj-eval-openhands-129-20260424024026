#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

using namespace std;

struct Variable {
    string type;
    variant<int, string> value;
};

class ScopeManager {
private:
    vector<unordered_map<string, Variable>> scopes;

public:
    ScopeManager() {
        scopes.push_back(unordered_map<string, Variable>());
    }

    void indent() {
        scopes.push_back(unordered_map<string, Variable>());
    }

    bool dedent() {
        if (scopes.size() <= 1) {
            return false;
        }
        scopes.pop_back();
        return true;
    }

    bool declare(const string& type, const string& var_name, const string& value_str) {
        // Check if variable already exists in current scope
        if (scopes.back().count(var_name)) {
            return false;
        }

        Variable var;
        var.type = type;
        
        if (type == "int") {
            try {
                var.value = stoi(value_str);
            } catch (...) {
                return false;
            }
        } else if (type == "string") {
            // Remove quotes
            if (value_str.size() < 2 || value_str.front() != '"' || value_str.back() != '"') {
                return false;
            }
            var.value = value_str.substr(1, value_str.size() - 2);
        } else {
            return false;
        }

        scopes.back()[var_name] = var;
        return true;
    }

    Variable* find_variable(const string& var_name) {
        for (int i = scopes.size() - 1; i >= 0; i--) {
            if (scopes[i].count(var_name)) {
                return &scopes[i][var_name];
            }
        }
        return nullptr;
    }

    bool add(const string& result, const string& value1, const string& value2) {
        Variable* res = find_variable(result);
        Variable* val1 = find_variable(value1);
        Variable* val2 = find_variable(value2);

        if (!res || !val1 || !val2) {
            return false;
        }

        if (res->type != val1->type || res->type != val2->type) {
            return false;
        }

        if (res->type == "int") {
            res->value = get<int>(val1->value) + get<int>(val2->value);
        } else {
            res->value = get<string>(val1->value) + get<string>(val2->value);
        }

        return true;
    }

    bool self_add(const string& var_name, const string& value_str) {
        Variable* var = find_variable(var_name);
        if (!var) {
            return false;
        }

        if (var->type == "int") {
            try {
                int add_value = stoi(value_str);
                var->value = get<int>(var->value) + add_value;
            } catch (...) {
                return false;
            }
        } else if (var->type == "string") {
            if (value_str.size() < 2 || value_str.front() != '"' || value_str.back() != '"') {
                return false;
            }
            string add_str = value_str.substr(1, value_str.size() - 2);
            var->value = get<string>(var->value) + add_str;
        } else {
            return false;
        }

        return true;
    }

    bool print(const string& var_name) {
        Variable* var = find_variable(var_name);
        if (!var) {
            return false;
        }

        cout << var_name << ":";
        if (var->type == "int") {
            cout << get<int>(var->value) << "\n";
        } else {
            cout << "\"" << get<string>(var->value) << "\"" << "\n";
        }

        return true;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n;
    cin >> n;

    ScopeManager manager;

    for (int i = 0; i < n; i++) {
        string cmd;
        cin >> cmd;
        
        bool valid = true;

        if (cmd == "Indent") {
            manager.indent();
        } else if (cmd == "Dedent") {
            valid = manager.dedent();
        } else if (cmd == "Declare") {
            // Parse: Declare [type] [variable_name] [value]
            string type, var_name, value;
            cin >> type >> var_name;
            
            if (type == "int") {
                cin >> value;
            } else if (type == "string") {
                // Read quoted string
                cin >> ws; // Skip whitespace
                if (cin.peek() == '"') {
                    cin.get(); // Skip opening quote
                    getline(cin, value, '"');
                    value = "\"" + value + "\"";
                } else {
                    valid = false;
                }
            } else {
                valid = false;
            }
            
            if (valid) {
                valid = manager.declare(type, var_name, value);
            }
        } else if (cmd == "Add") {
            // Parse: Add [result] [value1] [value2]
            string result, value1, value2;
            cin >> result >> value1 >> value2;
            
            valid = manager.add(result, value1, value2);
        } else if (cmd == "SelfAdd") {
            // Parse: SelfAdd [variable_name] [value]
            string var_name, value;
            cin >> var_name;
            
            // Peek to see if it's a string or int
            cin >> ws;
            if (cin.peek() == '"') {
                cin.get(); // Skip opening quote
                getline(cin, value, '"');
                value = "\"" + value + "\"";
            } else {
                cin >> value;
            }
            
            valid = manager.self_add(var_name, value);
        } else if (cmd == "Print") {
            // Parse: Print [variable_name]
            string var_name;
            cin >> var_name;
            valid = manager.print(var_name);
        } else {
            valid = false;
        }

        if (!valid) {
            cout << "Invalid operation\n";
        }
    }

    return 0;
}
