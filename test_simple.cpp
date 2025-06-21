#include <iostream>
#include <boost/program_options.hpp>

using namespace std;
using namespace boost::program_options;

int main(int argc, char** argv) {
    cout << "=== Simple Test Program ===" << endl;
    cout << "argc: " << argc << endl;
    
    for(int i = 0; i < argc; i++) {
        cout << "argv[" << i << "]: " << argv[i] << endl;
    }
    
    try {
        options_description desc("Test Options");
        desc.add_options()
            ("help,h", "Show help message")
            ("version,v", "Show version");
            
        variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);
        
        if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        }
        
        if (vm.count("version")) {
            cout << "Test Version 1.0" << endl;
            return 0;
        }
        
        cout << "No options provided. Use --help for usage." << endl;
        
    } catch (exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
} 