#pragma once
#include <string>
using namespace std;

template <class T>
class IRepository {
public:
    virtual ~IRepository() {}
    virtual T findById(int id) = 0;
    virtual bool save(T& entity) = 0;
    virtual bool remove(int id) = 0;
};