//
// Created by liu on 18-10-25.
//
// Modified by yuan on 20-10-24.

#ifndef PROJECT_QUERYRESULT_H
#define PROJECT_QUERYRESULT_H

#include "../utils/formatter.h"
#include "../db/Table.h"

#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>

class QueryResult {
public:
    typedef std::unique_ptr<QueryResult> Ptr;

    virtual bool success() = 0;

    virtual bool display() = 0;

    virtual ~QueryResult() = default;

    friend std::ostream &operator<<(std::ostream &os, const QueryResult &table);

protected:
    virtual std::ostream &output(std::ostream &os) const = 0;
};

class FailedQueryResult : public QueryResult {
    const std::string data;
public:
    bool success() override { return false; }

    bool display() override { return false; }
};

class SuceededQueryResult : public QueryResult {
public:
    virtual void merge(QueryResult::Ptr q) = 0;

    // some query need do final jobs after merge, like select
    virtual void finish() {}

    bool success() override { return true; }
};

class NullQueryResult : public SuceededQueryResult {
public:
    bool display() override { return false; }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "\n";
    }
};

class ErrorMsgResult : public FailedQueryResult {
    std::string msg;
public:
    bool display() override { return false; }

    ErrorMsgResult(const std::string &qname,
                   const std::string &msg) {
        this->msg = R"(Query "?" failed : ?)"_f % qname % msg;
    }

    ErrorMsgResult(const std::string &qname,
                   const std::string &table,
                   const std::string &msg) {
        this->msg =
                R"(Query "?" failed in Table "?" : ?)"_f % qname % table % msg;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

class SuccessMsgResult : public SuceededQueryResult {
    std::string msg;
public:
    bool display() override { return false; }

    void merge(QueryResult::Ptr q) override {}

    explicit SuccessMsgResult(const int number) {
        this->msg = R"(ANSWER = "?".)"_f % number;
    }

    explicit SuccessMsgResult(std::vector<int> results) {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ")";
        this->msg = ss.str();
    }

    explicit SuccessMsgResult(const std::string &qname) {
        this->msg = R"(Query "?" success.)"_f % qname;
    }

    SuccessMsgResult(const std::string &qname, const std::string &msg) {
        this->msg = R"(Query "?" success : ?)"_f % qname % msg;
    }

    SuccessMsgResult(const std::string &qname,
                     const std::string &table,
                     const std::string &msg) {
        this->msg = R"(Query "?" success in Table "?" : ?)"_f
                    % qname % table % msg;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

class RecordCountResult : public SuceededQueryResult {
    int affectedRows;
public:
    bool display() override { return true; }

    void merge(QueryResult::Ptr q) override {
      RecordCountResult *cr = dynamic_cast<RecordCountResult *>(q.get());
      affectedRows += cr->affectedRows;
    }

    explicit RecordCountResult(int count) : affectedRows(count) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "Affected ? rows."_f % affectedRows << "\n";
    }
};

/*
 * need to implmenet merge
 */
class RecordMessageResult : public SuceededQueryResult {
    std::string msg;

public:
    bool display() override { return true; }

    explicit RecordMessageResult(std::vector<int> results) {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") "; // strange <space> is added according to the standard bin
        this->msg = ss.str();
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << msg << "\n";
    }
};

/*
 * For SelectQuery
 *
 * 1) need to implmenet merge
 * 2) the data is unordered during merge, only when 
 * finish is called, the result is sorted
 */
class RecordSelectResult: public SuceededQueryResult {
    std::vector<std::vector<std::string>> results;
public:
    bool display() override { return true; }

    RecordSelectResult() {}
    
    RecordSelectResult(const std::vector<std::vector<std::string>>& results): results(results) {}
    
    RecordSelectResult(std::vector<std::vector<std::string>>&& results): results(std::move(results)) {}

    void merge(QueryResult::Ptr q) override {
      RecordSelectResult *sr = dynamic_cast<RecordSelectResult *>(q.get());
      results.insert(results.end(), std::make_move_iterator(sr->results.begin()), std::make_move_iterator(sr->results.end()));
    }

    // sort results
    void finish() override {
        using namespace std;
        sort(results.begin(), results.end(), [](const vector<string>& s1, const vector<string>& s2){
            return s1[0]<s2[0];
        });
    }

protected:
    std::ostream& output(std::ostream& os) const override {
        for (const std::vector<std::string>& row: results) {
            os<<"( ";
            for (const std::string& elem: row) {
                os<<elem<<" ";
            }
            os<<")\n";
        }
        return os;
    }
};

/*
 * need to implmenet merge
 */
class CountNumberResult : public SuceededQueryResult {
    int affectedRows;
public:
    void merge(QueryResult::Ptr q) override {
      CountNumberResult *cr = dynamic_cast<CountNumberResult *>(q.get());
      affectedRows += cr->affectedRows;
    }

    bool display() override { return true; }

    explicit CountNumberResult(int count) : affectedRows(count) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "ANSWER = ?"_f % affectedRows << "\n";
    }
};

class AffectNumberResult : public SuceededQueryResult {
    int affectedRows;
public:
    void merge(QueryResult::Ptr q) override {
        AffectNumberResult *cr = dynamic_cast<AffectNumberResult *>(q.get());
        affectedRows += cr->affectedRows;
    }

    bool display() override { return true; }

    explicit AffectNumberResult(int count) : affectedRows(count) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "Affected ? rows."_f % affectedRows << "\n";
    }
};

class SumNumberResult : public SuceededQueryResult {
    std::vector<int> results;
public:
    explicit SumNumberResult(std::vector<int> answers){
        results=move(answers);
        //this->results.assign(answers.begin(),answers.end());
    }

    void merge(QueryResult::Ptr q) override {
        auto *cr = dynamic_cast<SumNumberResult *>(q.get());
        size_t index=0;
        while (index < cr->results.size()) {
            this->results[index] += cr->results[index];
            index++;
        }

    }

    bool display() override { return true; }

protected:
    std::ostream &output(std::ostream &os) const override {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") "; // strange <space> is added according to the standard bin
        std::string msg = ss.str();
        return os << msg << "\n";
    }
};


class MaxNumberResult : public SuceededQueryResult {
    bool has;
    std::vector<int> results;
public:
    explicit MaxNumberResult(bool isHas, std::vector<int> answers){
        has=isHas;
        results=move(answers);
        //this->results.assign(answers.begin(),answers.end());
    }

    void merge(QueryResult::Ptr q) override {
        auto *cr = dynamic_cast<MaxNumberResult *>(q.get());
        this->has=this->has || cr->has;
        size_t index=0;
        while (index < cr->results.size()&&cr->has!=false) {
            this->results[index] = (this->results[index] < cr->results[index]) ? cr->results[index]
                                                                               : this->results[index];
            index++;
        }

    }

    bool display() override {
        if(this->has)
            return true;
        else
            return false;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") "; // strange <space> is added according to the standard bin
        std::string msg = ss.str();
        return os << msg << "\n";
    }
};

class MinNumberResult : public SuceededQueryResult {
    bool has;
    std::vector<int> results;
public:
    explicit MinNumberResult(bool isHas, std::vector<int> answers){
        has=isHas;
        results=move(answers);
        //this->results.assign(answers.begin(),answers.end());
    }

    void merge(QueryResult::Ptr q) override {
        auto *cr = dynamic_cast<MinNumberResult *>(q.get());
        this->has=this->has || cr->has;
        size_t index=0;
        while (index < cr->results.size()&&cr->has!=false) {
            this->results[index] = (this->results[index] > cr->results[index]) ? cr->results[index]
                                                                               : this->results[index];
            index++;
        }

    }

    bool display() override {
        if(this->has)
            return true;
        else
            return false;
    }

protected:
    std::ostream &output(std::ostream &os) const override {
        std::stringstream ss;
        ss << "ANSWER = ( ";
        for (auto result : results) {
            ss << result << " ";
        }
        ss << ") "; // strange <space> is added according to the standard bin
        std::string msg = ss.str();
        return os << msg << "\n";
    }
};


/*
 * DuplicateQuery parital result
 */
class DuplicateQueryTaskResult : public SuceededQueryResult {
public:
  std::vector<Table::Datum> v;

  Table::SizeType size() {return v.size(); };

  void merge(QueryResult::Ptr q) override {}

  bool display() override {return true;}

  explicit DuplicateQueryTaskResult(std::vector<Table::Datum> &&v): v(v)  {}

  std::ostream &output(std::ostream &os) const override {
    return os << "dup task result\n";
  }
};

/*
 * DeleteQueryPartialResult parital result
 */
class DeleteQueryPartialResult : public SuceededQueryResult {
public:
  std::vector<Table::KeyType> v;

  Table::SizeType size() {return v.size(); };

  void merge(QueryResult::Ptr q) override {}

  bool display() override {return true;}

  explicit DeleteQueryPartialResult(std::vector<Table::KeyType> &&v): v(v)  {}

  std::ostream &output(std::ostream &os) const override {
    return os << "delete task result\n";
  }
};

class ListenQueryResult : public SuceededQueryResult {
    std::string filename;
public:
    void merge(QueryResult::Ptr q) override {}

    bool display() override { return true; }

    explicit ListenQueryResult(std::string filename) : filename(filename) {}

protected:
    std::ostream &output(std::ostream &os) const override {
        return os << "ANSWER = ( listening from ? )"_f % filename << "\n";
    }
};

#endif //PROJECT_QUERYRESULT_H
