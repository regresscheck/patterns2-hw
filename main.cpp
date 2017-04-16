#include <iostream>
#include <map>
#include <string>
#include <vector>


class ITupleField {
public:
    virtual void* get() = 0;
    virtual ITupleField* copyField() const = 0;
    virtual ~ITupleField() {};
};

template<class T>
class TTupleField: public ITupleField {
private:
    T value;
public:
    TTupleField() {}
    virtual void* get() {
        return &value;
    }
    virtual ITupleField* copyField() const {
        return new TTupleField<T>();
    };
};

class TTuple;


class TTuple {
private:
    std::vector<ITupleField*> fields;
public:
    class TTupleSupportedTypes {
    private:
        friend TTuple;
        std::map<std::string, ITupleField*> field_types;
        TTupleSupportedTypes() {}
        ~TTupleSupportedTypes() {
            for (auto field_type : field_types) {
                delete field_type.second;
            }
        }
        TTupleSupportedTypes(const& TTupleSupportedTypes) = delete;
        TTupleSupportedTypes& operator=(const& TTupleSupportedTypes) = delete;
        template<typename T>
        void addType(const std::string& type_name) {
            field_types[type_name] = new TTupleField<T>();
        }
        ITupleField* createField(const std::string& type_name) {
            return field_types[type_name]->copyField();
        }
        static TTupleSupportedTypes& Instance() {
            static TTupleSupportedTypes singleton;
            return singleton;
        }
        static ITupleField* CreateField(const std::string& type_name) {
            TTupleSupportedTypes::Instance().createField(type_name);
        }
    public:
        template<typename T>
        static void AddType(const std::string& type_name) {
            TTupleSupportedTypes::Instance().addType<T>(type_name);
        }
    };
    TTuple(std::vector<std::string> type_names) {
        for (const std::string& type_name : type_names) {
            fields.push_back(TTupleSupportedTypes::CreateField(type_name));
        }
    }
    template<typename T>
    T& get(int index) {
        return *reinterpret_cast<T*>(fields[index]->get());
    }
    void* operator[](int index) {
        return fields[index]->get();
    }

    virtual ~TTuple() {
        for (auto field_reference : fields) {
            delete field_reference;
        }
    }
};


int main()
{
    TTuple::TTupleSupportedTypes::AddType<int>("int");
    TTuple::TTupleSupportedTypes::AddType<double>("double");
    TTuple::TTupleSupportedTypes::AddType<bool>("bool");
    std::vector<std::string> types;
    types.push_back("int");
    types.push_back("double");
    types.push_back("int");
    types.push_back("bool");
    TTuple t = TTuple(types);
    int* first = reinterpret_cast<int*>(t[0]);
    *first = 1;
    int* second = reinterpret_cast<int*>(t[2]);
    *second = 2;
    *reinterpret_cast<bool*>(t[3]) = true;
    std::cout << t.get<int>(0) << std::endl;
    std::cout << t.get<int>(2) << std::endl;
    std::cout << t.get<bool>(3) << std::endl;
    return 0;
}
