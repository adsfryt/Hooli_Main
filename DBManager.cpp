#include <iostream>
#include <pqxx/pqxx>
#include "json.hpp"
#include <string>
using json = nlohmann::json;

namespace DBManager
{
    static nlohmann::json GetSubjectRow(int id,pqxx::work &w){
         std::string querySQL = "SELECT * FROM subjects WHERE id=";
        querySQL += std::to_string(id);
        querySQL += " LIMIT 1";
        json responce = json::object();
        pqxx::result R = w.exec(querySQL);
        
        for (auto const &row : R) {
            json obj = json::parse(row["data"].as<std::string>());
            if(!(obj.contains("disable") &&  obj["disable"])){
                
                responce = obj;
                responce["id"] = id;
            }
        }
        return responce;
    }

    static int UpdateSubjectRow(int id, json row, pqxx::work &w){
        std::string querySQL = "UPDATE subjects SET data = ";
        querySQL += w.quote(row.dump());
        querySQL += " WHERE id = ";
        querySQL += std::to_string(id);
        w.exec(querySQL);
        return 1;
    };

    static int AddSubjectRow( json row, pqxx::work &w){
         std::string querySQL = "INSERT INTO subjects (data) VALUES ( ";
        querySQL +=  w.quote(row.dump());
        querySQL += " )  RETURNING id;";
        pqxx::result R = w.exec(querySQL);
        int responce = -1;
        std::cout <<responce << std::endl;
        if (!R.empty()) {
            responce = R[0][0].as<int>();
        }
        std::cout <<responce << std::endl;
        return responce;
    }



    static int AddTestRow( json row, pqxx::work &w){
         std::string querySQL = "INSERT INTO tests (data) VALUES ( ";
        querySQL +=  w.quote(row.dump());
        querySQL += " )  RETURNING id;";
        pqxx::result R = w.exec(querySQL);
        int responce = -1;
        if (!R.empty()) {
            responce = R[0][0].as<int>();
        }
        return responce;
    }

    static nlohmann::json GetTestRow(int id,pqxx::work &w){
         std::string querySQL = "SELECT * FROM tests WHERE id=";
        querySQL += std::to_string(id);
        querySQL += " LIMIT 1";
        json responce = json::object();
        pqxx::result R = w.exec(querySQL);
        for (auto const &row : R) {
            json obj = json::parse(row["data"].as<std::string>());
            if(!(obj.contains("disable") &&  obj["disable"])){

                responce = json::parse(row["data"].as<std::string>());
                responce["id"] = id;
                
            }
        }
        return responce;
    }
    
    static int UpdateTestRow(int id, json row, pqxx::work &w){
         std::string querySQL = "UPDATE tests SET data = ";
        querySQL += w.quote(row.dump());
        querySQL += " WHERE id = ";
        querySQL += std::to_string(id);
        w.exec(querySQL);
        return 1;
    };



    static int SaveAnswer(int id, json row, pqxx::work &w){

         std::string querySQL = "UPDATE attempts SET data = ";
        querySQL += w.quote(row.dump());
        querySQL += " WHERE id = ";
        querySQL += std::to_string(id);
        w.exec(querySQL);
        return 1;
    };

    static int AddAttemptRow( int testId, std::string userId, json lastsId, pqxx::work &w){
        json row = std::vector<int>(lastsId.size(),-1);

        std::string querySQL = "INSERT INTO attempts ( testId, userId , finished , lastsId , data , result) VALUES ( ";
        
        querySQL += std::to_string(testId);
        querySQL += ",";
        
        querySQL +=  w.quote(userId);
        querySQL += ",";

        querySQL += "false";
        querySQL += ",";  

        querySQL +=  w.quote(lastsId.dump());
        querySQL += ",";

        querySQL += w.quote(row.dump());
        querySQL += ",";

        querySQL += "0";
        querySQL += ")  RETURNING id;";
        pqxx::result R = w.exec(querySQL);
        int responce = -1;
        if (!R.empty()) {
            responce = R[0][0].as<int>();
        }
        return responce;
    }
 
    static int AddQuestionRow( int subjectId, json row, pqxx::work &w){

        std::string querySQL = "INSERT INTO questions ( subjectId, data ) VALUES ( ";
        
        querySQL += std::to_string(subjectId);
        querySQL += ",";

        querySQL += w.quote(row.dump());

        querySQL += ")  RETURNING id;";
        pqxx::result R = w.exec(querySQL);
        int responce = -1;
        if (!R.empty()) {
            responce = R[0][0].as<int>();
        }
        return responce;
    }
 
    static int UpdateQuestionRow(int id, json row, pqxx::work &w){
         std::string querySQL = "UPDATE questions SET data = ";
        querySQL += w.quote(row.dump());
        querySQL += " WHERE id = ";
        querySQL += std::to_string(id);
        w.exec(querySQL);
        return 1;
    };



     static int StopAttempt(long long id, long long result, pqxx::work &w){

         std::string querySQL = "UPDATE attempts SET finished = ";
        querySQL += "true";
        querySQL += " WHERE id = ";
        querySQL += std::to_string(id);
        w.exec(querySQL);

        std::string querySQL1 = "UPDATE attempts SET result = ";
        querySQL1 += std::to_string(result);
        querySQL1 += " WHERE id = ";
        querySQL1 += std::to_string(id);
        w.exec(querySQL1);

        return 1;
    };

};

