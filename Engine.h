#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <vector>
#include "BST.h"
#include "Record.h"

using namespace std;

static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

struct Engine {
    vector<Record> heap;
    BST<int, int> idIndex;
    BST<string, vector<int>> lastIndex;


    int insertRecord(const Record &recIn) {
        int rid = heap.size();
        heap.push_back(recIn);

        idIndex.insert(recIn.id, rid);

        string lname = toLower(recIn.last);
        vector<int>* vec = lastIndex.find(lname);

        if (!vec) {
            vector<int> tmp;
            tmp.push_back(rid);
            lastIndex.insert(lname, tmp);
        } else {
            vec->push_back(rid);
        }

        return rid;
    }

   
    bool deleteById(int id) {
        idIndex.resetMetrics();

        int* ridPtr = idIndex.find(id);
        if (!ridPtr) return false;

        int rid = *ridPtr;
        Record &rec = heap[rid];
        if (rec.deleted) return false;

        rec.deleted = true;
        idIndex.erase(id);

        string lname = toLower(rec.last);
        vector<int>* vec = lastIndex.find(lname);

        if (vec) {
            vector<int>& v = *vec;
            for (int i = 0; i < v.size(); i++) {
                if (v[i] == rid) {
                    for (int j = i; j < v.size() - 1; j++) {
                        v[j] = v[j + 1];
                    }
                    v.pop_back();
                    break;
                }
            }

            if (v.empty())
                lastIndex.erase(lname);
        }

        return true;
    }


    const Record *findById(int id, int &cmpOut) {
        idIndex.resetMetrics();
        int* ridPtr = idIndex.find(id);
        cmpOut = idIndex.comparisons;

        if (!ridPtr) return nullptr;

        const Record &rec = heap[*ridPtr];
        if (rec.deleted) return nullptr;

        return &rec;
    }

   
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        vector<const Record *> result;
        idIndex.resetMetrics();

        idIndex.rangeApply(lo, hi, [&](int key, int rid) {
            const Record &rec = heap[rid];
            if (!rec.deleted)
                result.push_back(&rec);
        });

        cmpOut = idIndex.comparisons;
        return result;
    }

    
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        vector<const Record *> result;
        lastIndex.resetMetrics();

        string lowPref = toLower(prefix);
        string hiPref = lowPref;

        if (!hiPref.empty()) hiPref.back()++;

        lastIndex.rangeApply(lowPref, hiPref, [&](const string &lname, vector<int> &vec) {
            if (lname.rfind(lowPref, 0) != 0) return;

            for (int rid : vec) {
                const Record &rec = heap[rid];
                if (!rec.deleted)
                    result.push_back(&rec);
            }
        });

        cmpOut = lastIndex.comparisons;
        return result;
    }
};

#endif
