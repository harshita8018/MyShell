#include<iostream>
#include<unistd.h>
#include<pwd.h>
#include<sys/utsname.h>
#include<vector>
#include<dirent.h>
#include<sys/stat.h>
#include<grp.h>
#include<fstream>
#include<fcntl.h>
#include<string.h>
#include<sys/wait.h>

using namespace std;

void bg_sys_call(vector<string>& cmd_arr)
{
    char* copyy[cmd_arr.size() + 1];
    for (int  i = 0; i < cmd_arr.size(); ++i) {
        copyy[i] = strdup(cmd_arr[i].c_str());
    }
    copyy[cmd_arr.size()] = NULL;

    int p_id = fork();
    if (p_id == 0)
    {
        if (execvp(copyy[0], copyy) == -1) 
        {
            cout<<"ERRORRRRRRRRRR"<<endl;
        }
    }
    else if(p_id > 0) 
    {
        cout << "Background process ID: " << p_id << endl;
        for (int i = 0; i < cmd_arr.size(); i++)
            free(copyy[i]);
    }
    else 
    {
        cout<<"ERROR"<<endl;
    }
}

void fg_sys_call(vector<string>& cmd_arr)
{
    char *copy_cmd[cmd_arr.size() + 1];
    for (int i = 0; i < cmd_arr.size(); ++i) {
        copy_cmd[i] = strdup(cmd_arr[i].c_str());
    }
    copy_cmd[cmd_arr.size()] = NULL;
    pid_t pid = fork();
    if (pid == 0) 
    {
        if (execvp(cmd_arr[0].c_str(), copy_cmd) == -1) 
        {
            perror("Error");
        }
    }
    else if (pid > 0) 
    {
        int status;
        waitpid(pid, &status, 0);
        for (int i = 0; i < cmd_arr.size(); i++) {
            free(copy_cmd[i]);
        }
    } 
    else 
    {
        perror("fork");
    }
}

void get_file_names(const string& cur_dir_path, vector<string>& file_names) 
{
    DIR* dir = opendir(cur_dir_path.c_str());
    if (dir == NULL ) 
        return ;
    dirent* dir_data;
    while ((dir_data = readdir(dir)) != nullptr) 
    {
        string file_name = dir_data->d_name;

        if ( file_name[0] != '.' ) 
        {
            string filePath = cur_dir_path + "/" + file_name;

            if (dir_data->d_type == DT_DIR) 
            {
                file_names.push_back(file_name);
                get_file_names(filePath, file_names);
            } 
            else if (dir_data->d_type == DT_REG) 
            {
                file_names.push_back(file_name);
            }
        }
    }
    closedir(dir);
}

vector<string> cmd_break(string s, char del)
{
    vector<string> ans;
    int j = 0;
    for(int i = 0; i < s.size(); i++)
    {
        if(s[i]==del)
        {
            //cout<<"17"<<del<<s.substr(j, i-1)<<endl;
            //cout<<"18"<<endl;
            ans.push_back(s.substr(j, i-j));
            j = i+1;
            while(s[j++]==' ' );
            j--;
        }
    }
    ans.push_back(s.substr(j, s.size()));
    int k = 0;
        //ans.erase(std::remove(ans.begin(), myVector.end(), 4), myVector.end());
    while( k < ans.size() )
    {
        if( ans[k] == "" || ans[k] == " " )
            ans.erase(ans.begin() + k);
        else
            k++;
    }
    return ans;
}

string u_name() 
{
    uid_t uid = geteuid();
    struct passwd *data = getpwuid(uid);
    return data->pw_name;
}

string s_name() 
{
    struct utsname data;
    uname(&data);
    return data.nodename;
}

string u_pwd() {
    char arr[100];
    getcwd(arr, sizeof(arr));
    return arr;
}

int main() 
{
    vector<string> cmd_history;
    int loop_var = 1, fg_bg = 0;
    string cur_dir = u_pwd();
    string home_path = cur_dir;
    string pre_dir = cur_dir;
    while (loop_var) 
    {
        try
        {
            int his = 1;
            string user = u_name();
            string sys_name = s_name();
            cur_dir = u_pwd();
            string display_dir = "";
            int pos_ind = cur_dir.find(home_path);
            int flag = 0;                                               
            try
            {
                display_dir = cur_dir.substr(pos_ind + home_path.length());
            }
            catch(const std::exception& e)
            {
                //std::cerr << e.what() << '\n';
                display_dir = "";
                //cout<<cur_dir<<endl;
                cur_dir = home_path;
                flag = 1;
            }
            cout<<user<<"@"<<sys_name<<":~"<<display_dir<<">";
            string cmd;
            vector<string> all_cmd_arr;
            vector<string> cmd_arr;
            getline(cin, cmd);
            //CTRL DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
            if (cin.eof()) 
            {
                cout<<endl;
                break;
            }
            //cin>>cmd;
            all_cmd_arr = cmd_break(cmd, ';');
            // cout<<all_cmd_arr.size()<<endl;
            for(int k=0; k < all_cmd_arr.size(); k++)
            {
                if(all_cmd_arr[k]=="" || all_cmd_arr[k]=="")
                    continue;
                cmd_arr = cmd_break(all_cmd_arr[k], ' ');
                if( cmd_history.size() < 20 )
                    cmd_history.insert(cmd_history.begin(), cmd);
                else
                {
                    cmd_history.insert(cmd_history.begin(), cmd);
                    cmd_history.pop_back();
                }
                //REDIRECTIONNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN
                int redirection_flag = 0;
                int input_re_flag = 0;
                int output_re_flag_append = 0;
                int output_re_flag_overrite = 0;
                if (cmd.find("<") != string::npos) 
                {
                    redirection_flag = 1;
                    input_re_flag = 1;
                }
                else if (cmd.find(">") != string::npos) 
                {
                    redirection_flag = 1;
                    output_re_flag_overrite = 1;
                }
                else if (cmd.find(">>") != string::npos) 
                {
                    redirection_flag = 1;
                    output_re_flag_append = 1;
                }
                try
                {
                
                    if(input_re_flag == 1)
                    {
                        int fd1=open(cmd_arr[2].c_str(), O_RDONLY);
                        if(fd1 == -1)
                        {
                            cout<<"Can't Open the input file"<<endl;
                        }
                        if( cmd_arr[0] == "cat" )
                        {
                            char line[100];
                            while(read(fd1, line, sizeof(line)))
                            {
                                cout<<line<<endl;
                            }
                        }
                    }
                    if( output_re_flag_overrite == 1 || output_re_flag_append == 1 )
                    {
                        int fd1;
                        if(output_re_flag_overrite)
                            fd1 = open(cmd_arr[3].c_str(), O_CREAT |  O_WRONLY | O_TRUNC, 0664 );
                        if( output_re_flag_append )
                            fd1 = open(cmd_arr[3].c_str(), O_CREAT |  O_WRONLY | O_APPEND , 0664 );
                        if(fd1 == -1)
                        {
                            cout<<"Can't Open/Create the output file"<<endl;
                        }
                        else if( cmd_arr[0] == "echo" )
                        {
                            write(fd1, cmd_arr[1].c_str(), strlen(cmd_arr[1].c_str()));
                        }
                    }
                }
                catch(const std::exception& e)
                {
                    std::cerr << e.what() << '\n';
                    cout<<"Something Went Wrong"<<endl;
                }

                //HISTORYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY
                if(cmd_arr[0] == "history")
                {
                    his = 0;
                    try
                    {
                        if(cmd_arr.size() == 1)
                        {
                            for(int i = 0; i < min(10, (int)cmd_history.size()) ; i++ )
                                    cout<<cmd_history[i]<<endl;
                        }
                        else if(cmd_arr.size() == 2 && cmd_history.size() < stoi(cmd_arr[1])  )
                            cout<<"No of command availabe : "<<cmd_history.size()<<endl;
                        else if( cmd_arr.size() == 2 )
                        {
                            try
                            {
                                for(int i = 0; i < stoi(cmd_arr[1]); i++ )
                                    cout<<cmd_history[i]<<endl;
                            }
                            catch(const std::exception& e)
                            {
                                cout<<"Incorrect Usage of History"<<endl;
                            }
                            
                        }
                    }
                    catch(const std::exception& e)
                    {
                        cout<<"Incorrect Usage of History"<<endl;
                    }
                }
                //EXITTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
                if(cmd_arr[0]=="exit") 
                {
                    loop_var = 0;
                    break;
                }
                //ECHOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
                else if(cmd_arr[0] == "echo" && redirection_flag == 0 )
                {
                    if(cmd_arr.size() == 1)
                        cout<<endl;
                    for(int x = 1; x<cmd_arr.size(); x++)
                        cout<<cmd_arr[x]<<" ";
                    cout<<endl;
                }
                //PWDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
                else if(cmd_arr[0]=="pwd")
                {
                    cout<<cur_dir<<endl;
                }
                //CDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD
                else if(cmd_arr[0] == "cd" )
                {
                    if(cmd_arr.size() == 1)
                    {
                        flag = 1;
                        const char *home_dir = home_path.c_str();
                        chdir(home_dir);
                    }
                    else if( cmd_arr.size() > 2 )
                    {
                        flag = 1;
                            cout<<"Invalid arguments for error handling"<<endl;
                    }
                    else if( cmd_arr[1] == "-" )
                    {
                        flag = 1;
                        const char *dir_path = pre_dir.c_str();
                        chdir(dir_path);
                        cout<<pre_dir<<endl;
                    }
                    else if( cmd_arr[1] == ".." )
                    {
                        flag = 1;
                        if(display_dir=="")
                            cout<<cur_dir<<endl;
                        else
                        {
                            const char *dir_path = cmd_arr[1].c_str();
                            chdir(dir_path);
                        }
                    }
                    else if( cmd_arr[1] == "~" || cmd_arr[1]==" " || cmd_arr[1] == "")
                    {
                        flag = 1;
                        const char *home_dir = home_path.c_str();
                        chdir(home_dir);
                    }
                    //const char *dir_path = cmd_arr[1].c_str();
                    else if( chdir(cmd_arr[1].c_str()) == -1 && flag == 0)
                    {
                        //const char *dir_path = cmd_arr[1].c_str();
                        cout<<"Invalid Path"<<endl;
                    }
                    // else
                    //     cout<<"cd : Invalid Option"<<endl;
                }
                //LSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS
                else if(cmd_arr[0] == "ls" )
                {
                    struct stat file_stat;
                    if( cmd_arr.size() == 1 || cmd_arr[1] == ".")
                    {
                        flag = 1;
                        const char* cur_directory = "."; 
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        else 
                        {
                            //cout<<"190";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else if( cmd_arr[1] == "-a" && cmd_arr.size() == 2)
                    {
                        flag = 1;
                        const char* cur_directory = "."; 
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                //if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        else 
                        {
                            //cout<<"210";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else if( cmd_arr[1] == ".." )
                    {
                        flag = 1;
                        if(display_dir=="")
                        {
                            const char* cur_directory = ".";
                        }
                        else
                        {
                            const char *dir_path = cmd_arr[1].c_str();
                            chdir(dir_path);
                        }
                        const char* cur_directory = ".";
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        const char *dir_path = cur_dir.c_str();
                        chdir(dir_path);
                    }
                    else if( cmd_arr[1] == "~" )
                    {
                        const char *dir_pathh = home_path.c_str();
                        chdir(dir_pathh);
                        const char* cur_directory = ".";
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        const char *dir_path = cur_dir.c_str();
                        chdir(dir_path);
                    }
                    else if( cmd_arr[1] == "-l" && cmd_arr.size() == 2)
                    {
                        flag = 1;
                        const char* cur_directory = "."; 
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.')
                                {
                                    struct stat file_stat;
                                    //char file_path[1024];
                                    string file_name = dir_data->d_name;
                                    string file_path = string(cur_directory) + "/" + file_name;

                                    if (lstat(file_path.c_str(), &file_stat) == 0) 
                                    {
                                        cout<<(S_ISDIR(file_stat.st_mode) ? "d" : "-");
                                        cout<<((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IROTH) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXOTH) ? "x" : "-")<<" ";

                                        cout<<file_stat.st_nlink<<" ";

                                        struct passwd* pw = getpwuid(file_stat.st_uid);
                                        cout<<pw->pw_name<<" ";
                                        
                                        struct group* gr = getgrgid(file_stat.st_gid);
                                        cout<<gr->gr_name<<" ";
                                        
                                        cout<<file_stat.st_size<<" ";
                                        
                                        struct tm* timeinfo = localtime(&file_stat.st_mtime);
                                        char time_str[80];
                                        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
                                        cout<<time_str << " ";

                                        cout<<dir_data->d_name<<" ";
                                    }


                                cout<<endl;

                                }
                            }
                            closedir(dir);
                        }
                        
                        else 
                        {
                            //cout<<"324";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else if( (cmd_arr[1] == "-l" && cmd_arr.size() == 3 && cmd_arr[2] == "-a") || (cmd_arr[1] == "-a" && cmd_arr.size() == 3 && cmd_arr[2] == "-l") || (cmd_arr[1] == "-al" && cmd_arr.size() == 2) || ( cmd_arr[1] == "-la" && cmd_arr.size() == 2 ) )
                    {
                        flag = 1;
                        const char* cur_directory = "."; 
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.' || dir_data->d_name[0] == '.')
                                {
                                    struct stat file_stat;
                                    //char file_path[1024];
                                    string file_name = dir_data->d_name;
                                    string file_path = string(cur_directory) + "/" + file_name;

                                    if (lstat(file_path.c_str(), &file_stat) == 0) 
                                    {
                                        cout<<(S_ISDIR(file_stat.st_mode) ? "d" : "-");
                                        cout<<((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IROTH) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXOTH) ? "x" : "-")<<" ";

                                        cout<<file_stat.st_nlink<<" ";

                                        struct passwd* pw = getpwuid(file_stat.st_uid);
                                        cout<<pw->pw_name<<" ";
                                        
                                        struct group* gr = getgrgid(file_stat.st_gid);
                                        cout<<gr->gr_name<<" ";
                                        
                                        cout<<file_stat.st_size<<" ";
                                        
                                        struct tm* timeinfo = localtime(&file_stat.st_mtime);
                                        char time_str[80];
                                        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
                                        cout<<time_str << " ";

                                        cout<<dir_data->d_name<<" ";
                                    }

                                cout<<endl;

                                }
                            }
                            closedir(dir);
                        }
                        else 
                        {
                            //cout<<"387";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else if( cmd_arr[1][0] != '-' && cmd_arr.size() == 3)
                    {
                        flag = 1;
                        const char* cur_directory = cmd_arr[1].c_str(); 
                        struct stat file_stat;
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        else if ( lstat(cmd_arr[1].c_str(), &file_stat) == 0 )
                        {
                                cout<<cmd_arr[1]<<endl;
                        }
                        else 
                        {
                            //cout<<"409";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else if( cmd_arr[1][0] == '-' && cmd_arr.size() == 3)
                    {
                        flag = 1;
                        const char* cur_directory = cmd_arr[2].c_str(); 
                        DIR* dir;
                        struct dirent* dir_data;
                        if ((dir = opendir(cur_directory)) != NULL && cmd_arr[1] == "-a" ) 
                        {
                            while ((dir_data = readdir(dir)) != NULL )
                            {
                                //if(dir_data->d_name[0] != '.')
                                    cout<<dir_data->d_name<<" ";
                            }
                            cout<<endl;
                            closedir(dir);
                        } 
                        else if( (dir = opendir(cmd_arr[2].c_str())) != NULL && cmd_arr[1] == "-l" )
                        {
                            const char* cur_directory = cmd_arr[2].c_str(); 
                            DIR* dir;
                            struct dirent* dir_data;
                            if ((dir = opendir(cur_directory)) != NULL) 
                            {
                                while ((dir_data = readdir(dir)) != NULL )
                                {
                                    if(dir_data->d_name[0] != '.')
                                    {
                                        struct stat file_stat;
                                        //char file_path[1024];
                                        string file_name = dir_data->d_name;
                                        string file_path = string(cur_directory) + "/" + file_name;

                                        if (lstat(file_path.c_str(), &file_stat) == 0) 
                                        {
                                            cout<<(S_ISDIR(file_stat.st_mode) ? "d" : "-");
                                            cout<<((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                                            cout<<((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                                            cout<<((file_stat.st_mode & S_IROTH) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXOTH) ? "x" : "-")<<" ";

                                            cout<<file_stat.st_nlink<<" ";

                                            struct passwd* pw = getpwuid(file_stat.st_uid);
                                            cout<<pw->pw_name<<" ";
                                            
                                            struct group* gr = getgrgid(file_stat.st_gid);
                                            cout<<gr->gr_name<<" ";
                                            
                                            cout<<file_stat.st_size<<" ";
                                            
                                            struct tm* timeinfo = localtime(&file_stat.st_mtime);
                                            char time_str[80];
                                            strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
                                            cout<<time_str << " ";

                                            cout<<dir_data->d_name<<" ";
                                        }


                                    cout<<endl;

                                    }
                                }
                                closedir(dir);
                            }
                        }
                        else if( (dir = opendir(cmd_arr[2].c_str())) != NULL && ( cmd_arr[1] == "-la" || cmd_arr[1] == "-al" ))
                        {
                            //cout<<"483"<<endl;
                            const char* cur_directory = cmd_arr[2].c_str(); 
                            DIR* dir;
                            struct dirent* dir_data;
                            struct stat file_stat;
                            if ((dir = opendir(cur_directory)) != NULL) 
                            {
                                while ((dir_data = readdir(dir)) != NULL )
                                {
                                    if(dir_data->d_name[0] != '.' || dir_data->d_name[0] == '.' )
                                    {
                                        struct stat file_stat;
                                        string file_name = dir_data->d_name;
                                        //cout<<file_name<<endl;
                                        string file_path = string(cur_directory) + "/" + file_name;

                                        if (lstat(file_path.c_str(), &file_stat) == 0) 
                                        {
                                            cout<<(S_ISDIR(file_stat.st_mode) ? "d" : "-");
                                            cout<<((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                                            cout<<((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                                            cout<<((file_stat.st_mode & S_IROTH) ? "r" : "-");
                                            cout<<((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                                            cout<<((file_stat.st_mode & S_IXOTH) ? "x" : "-")<<" ";

                                            cout<<file_stat.st_nlink<<" ";

                                            struct passwd* pw = getpwuid(file_stat.st_uid);
                                            cout<<pw->pw_name<<" ";
                                            
                                            struct group* gr = getgrgid(file_stat.st_gid);
                                            cout<<gr->gr_name<<" ";
                                            
                                            cout<<file_stat.st_size<<" ";
                                            
                                            struct tm* timeinfo = localtime(&file_stat.st_mtime);
                                            char time_str[80];
                                            strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
                                            cout<<time_str << " ";

                                            cout<<dir_data->d_name<<" ";
                                        }
                                    cout<<endl;
                                    }
                                }
                                closedir(dir);
                            }
                        }
                        else if(lstat(cmd_arr[2].c_str(), &file_stat) == 0)
                        {
                            if( cmd_arr[1] == "-a" )
                                cout<<cmd_arr[2]<<endl;
                            else if( cmd_arr[1] == "-l" || cmd_arr[1] == "-al" || cmd_arr[1] == "-la" )
                            {
                                cout<<(S_ISDIR(file_stat.st_mode) ? "d" : "-");
                                        cout<<((file_stat.st_mode & S_IRUSR) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWUSR) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXUSR) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IRGRP) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWGRP) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXGRP) ? "x" : "-");
                                        cout<<((file_stat.st_mode & S_IROTH) ? "r" : "-");
                                        cout<<((file_stat.st_mode & S_IWOTH) ? "w" : "-");
                                        cout<<((file_stat.st_mode & S_IXOTH) ? "x" : "-")<<" ";

                                        cout<<file_stat.st_nlink<<" ";

                                        struct passwd* pw = getpwuid(file_stat.st_uid);
                                        cout<<pw->pw_name<<" ";
                                        
                                        struct group* gr = getgrgid(file_stat.st_gid);
                                        cout<<gr->gr_name<<" ";
                                        
                                        cout<<file_stat.st_size<<" ";
                                        
                                        struct tm* timeinfo = localtime(&file_stat.st_mtime);
                                        char time_str[80];
                                        strftime(time_str, sizeof(time_str), "%b %d %H:%M", timeinfo);
                                        cout<<time_str << " ";

                                        cout<<cmd_arr[2]<<endl;

                            }
                            else
                                cout<<"ls : invalid option "<<cmd_arr[1]<<endl;
                        }
                        else 
                        {
                            //cout<<"562";
                            cout<<"No such file or directory"<<endl;
                        }
                    }
                    else
                        cout<<"ls : Invalid Option"<<endl;

                }
                //SEARCHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
                else if(cmd_arr[0] == "search")
                {
                    int tempp = 1;
                    if( cmd_arr.size() != 2)
                    {
                        cout<<"Incorrect Usage of Search Cmd"<<endl;
                        tempp = 0;
                    }
                    vector<string> file_names;
                    get_file_names(".", file_names);

                    // for(auto i: file_names)
                    //     cout<<i<<endl;

                    for(int i = 0; i < file_names.size() && tempp ; i++)
                        if(cmd_arr[1] == file_names[i])
                        {
                            tempp = 0;
                            cout<<"True"<<endl;
                            break;
                        }
                    if(tempp)
                        cout<<"False"<<endl;

                }
                //PINFOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
                else if(cmd_arr[0] == "pinfo")
                {
                    int temp_flag_check = 1;
                    pid_t pro_id = getpid();
                    if(cmd_arr.size() == 2)
                    {
                        try
                        {
                            pro_id = stoi(cmd_arr[1]);
                        }
                        catch(const std::exception& e)
                        {
                            std::cerr << e.what() << '\n';
                            temp_flag_check = 0;
                            cout<<"Incorrect Process ID"<<endl;
                        }
                    }
                    if(temp_flag_check)
                    {
                        string status_path = "/proc/" + to_string(pro_id) + "/status";
                        string exe_path = "/proc/" + to_string(pro_id) + "/exe";

                        string pro_status = "";
                        string pro_mem = "";
                        ifstream statusFile(status_path);
                        if (statusFile.is_open()) 
                        {
                            string line;
                            while (getline(statusFile, line)) 
                            {
                                //cout<<line<<endl;
                                if (line.find("State:") != string::npos) 
                                {
                                    //cout<<line<<endl;
                                    pro_status = line.substr(line.find(":") + 2);
                                    if(tcgetpgrp(STDIN_FILENO) == getpgid(pro_id) )
                                    {
                                        pro_status += "+";
                                    }
                                }
                                if (line.find("VmSize:") != string::npos) 
                                {
                                    pro_mem = line.substr(line.find(":") + 5);
                                }
                            }
                            statusFile.close();
                        } 
                        else 
                        {
                            cout<<"Operation Failed : Can't open status file"<<endl;
                        }
                        char exe_file_path[100];
                        int len = readlink(exe_path.c_str(), exe_file_path, sizeof(exe_path) - 1);
                        if (len == -1) 
                        {
                            cout<<"Operation Failed : Can't open exe file"<<endl;
                        } 

                        cout<<"pid -- "<<pro_id<<endl;
                        if(pro_status != "")
                            cout<<"Process Status -- "<<pro_status<<endl;
                        if(pro_mem != "")
                            cout<<"memory -- "<<pro_mem<<endl;
                        if(len != -1)
                            cout<<"Executable Path -- "<<exe_file_path<<endl;
                    }

                }
                //BG AND FGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGGG
                else if(his && redirection_flag == 0)
                {
                    fg_bg = 1;
                    if ( cmd_arr[cmd_arr.size() - 1] == "&") 
                    {
                        cmd_arr.pop_back();
                        bg_sys_call(cmd_arr);
                    } 
                    else 
                    {
                        fg_sys_call(cmd_arr);
                    }
                }
                else if(his && redirection_flag == 0 && fg_bg == 0)
                    cout<<all_cmd_arr[k]<<": command not found"<<endl;
            }
            pre_dir = cur_dir;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            cout<<"Something Went Wrong"<<endl;
        }
    }
    return 0;
}
