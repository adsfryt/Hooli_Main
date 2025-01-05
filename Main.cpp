#include <iostream>
#include <pqxx/pqxx>
#include "crow.h"
#include "json.hpp"
#include "DBManager.cpp"
using json = nlohmann::json;
int main()
{
  try
  {
    pqxx::connection c{"postgresql://test_owner:Dria5RXuJpF9@ep-dawn-wave-a2smzw1z.eu-central-1.aws.neon.tech/test?sslmode=require"};
    crow::SimpleApp app;
//проверить
 CROW_ROUTE(app, "/subject/add_subject")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);

        int res = DBManager::AddSubjectRow(data, w);
        std::cout << ".." << std::endl;
        if(res == -1){
            return crow::response(crow::status::BAD_REQUEST);  
        }

        w.commit();

        json resp;
        resp["subjectId"] = res;
        return crow::response{resp.dump()};
        
     });

 CROW_ROUTE(app, "/subject/get_data")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        for (size_t i = 0; i < data["subjectsId"].size(); i++)
        {
          std::string querySQL = "SELECT * FROM subjects WHERE id=";
          std::string id; 
          if(data["subjectsId"][i].is_number()){
            id = data["subjectsId"][i].dump();
          }
          else if(data["subjectsId"][i].is_string()){
            id = data["subjectsId"][i];
          }else{
            continue;
          }
          querySQL += id;

            querySQL += " LIMIT 1";
            pqxx::result R = w.exec(querySQL);

            for (auto const &row : R) {
              resp.push_back(json::parse(row["data"].as<std::string>()));
              resp[i]["id"] = std::stoi(id);
            }

        }
      
         
        w.commit();
        return crow::response{ resp.dump()};
     });




 CROW_ROUTE(app, "/subject/add_user")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        json row = DBManager::GetSubjectRow(data["subjectId"], w);
        if(row.empty()){
          return crow::response(crow::status::CONFLICT);  
        }
        for (size_t i = 0; i < row["usersId"].size(); i++)
        {
          if(row["usersId"][i] == data["userId"]){
            json resp;
            resp["ok"] = true;
            return crow::response{resp.dump()};
          }
        }
        
        row["usersId"].push_back(data["userId"]);
        DBManager::UpdateSubjectRow(data["subjectId"],row, w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{resp.dump()};
     });


 CROW_ROUTE(app, "/subject/update_data")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);

        json row = DBManager::GetSubjectRow(data["subjectId"], w);
        if(row.empty()){
          return crow::response(crow::status::CONFLICT);  
        }
        
        row["title"]  = data["title"];
        row["description"]  = data["description"];

        DBManager::UpdateSubjectRow(data["subjectId"],row, w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{resp.dump()};
     });



 CROW_ROUTE(app, "/subject/add_teacher")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        json row = DBManager::GetSubjectRow(data["subjectId"], w);
        if(row.empty()){
          return crow::response(crow::status::CONFLICT);  
        }
        for (size_t i = 0; i < row["userId"].size(); i++)
        {
          if(row["userId"][i] == data["userId"]){
            json resp;
            resp["ok"] = true;
            return crow::response{resp.dump()};
          }
        }
        
        row["userId"].push_back(data["userId"]);
        DBManager::UpdateSubjectRow(data["subjectId"],row, w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{resp.dump()};
     });


 CROW_ROUTE(app, "/subject/delete_user")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        json row = DBManager::GetSubjectRow(data["subjectId"], w);
        if(row.empty()){
          return crow::response(crow::status::CONFLICT);  
        }

        for (size_t i = 0; i < row["usersId"].size(); i++)
        {
          if(row["usersId"][i] == data["userId"]){
            row["usersId"].erase(i);
            break;
          }
        }

        DBManager::UpdateSubjectRow(data["subjectId"],row, w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{resp.dump()};
     });







 CROW_ROUTE(app, "/test/add_test")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);

        int res = DBManager::AddTestRow(data, w);
        std::cout << ".." << std::endl;
        if(res == -1){
            return crow::response(crow::status::BAD_REQUEST);  
        }
        json row = DBManager::GetSubjectRow(data["subjectId"], w);
        if(row.empty()){
          return crow::response(crow::status::CONFLICT);  
        }
        row["testsId"].push_back(res);
        DBManager::UpdateSubjectRow(data["subjectId"],row, w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{resp.dump()};
     });

 CROW_ROUTE(app, "/test/get_data")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        for (size_t i = 0; i < data["testsId"].size(); i++)
        {
            std::string querySQL = "SELECT * FROM tests WHERE id=";
            std::string id = data["testsId"][i].dump(); 
            querySQL += id;

            querySQL += " LIMIT 1";
            pqxx::result R = w.exec(querySQL);

            for (auto const &row : R) {
              resp.push_back(json::parse(row["data"].as<std::string>()));
              resp[i]["id"] = std::stoi(id);
            }
        }
      
        w.commit();
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/update_data")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);

        json test = DBManager::GetTestRow(data["testId"],w);

        if (data.find("name") != data.end())
        {
             test["name"] = data["name"];
        }
        if (data.find("description") != data.end())
        {
            test["description"] = data["description"];
        }
        if (data.find("activate") != data.end())
        {
            test["activate"] = data["activate"];
        }
        if (data.find("questionsId") != data.end())
        {
            test["questionsId"] = data["questionsId"];
        }
         test.erase("id");
        DBManager::UpdateTestRow(data["testId"],test,w);
        w.commit();
        json resp;

        resp["ok"] = true;
        return crow::response{ resp.dump()};
     });



 CROW_ROUTE(app, "/test/add_attempt")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        
        int id = DBManager::AddAttemptRow(data["testId"], data["userId"], data["lastsId"], w);
      
        w.commit();
        json resp;
        resp["attemptId"] = id;
        return crow::response{ resp.dump()};
     });
 CROW_ROUTE(app, "/test/save_answers")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);

        int id = DBManager::SaveAnswer(data["attemptId"], data["answers"], w);
      
        w.commit();
        json resp;
        resp["ok"] = true;
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/get_attempts")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        for (size_t i = 0; i < data["attemptsId"].size(); i++)
        {
            std::string querySQL = "SELECT * FROM attempts WHERE id=";
            std::string id = data["attemptsId"][i].dump(); 
            querySQL += id;

            querySQL += " LIMIT 1";
            pqxx::result R = w.exec(querySQL);

            for (auto const &row : R) {
              resp.push_back({});
              resp[i]["data"] = json::parse(row["data"].as<std::string>());
              resp[i]["id"] = std::stoi(id);
              resp[i]["testId"] = row["testid"].as<int>();
              resp[i]["userId"] = row["userid"].as<std::string>();
              resp[i]["lastsId"] = json::parse(row["lastsid"].as<std::string>());
              resp[i]["finished"] = row["finished"].as<bool>();
              resp[i]["result"] = row["result"].as<int>();
            }
        }
      
        w.commit();
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/get_active_attempt")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp;
        resp["attemptId"] = -1;
        std::string querySQL = "SELECT * FROM attempts WHERE testid=";
        std::string testId = data["testId"].dump(); 
        querySQL += testId;

        querySQL += " AND userid=";
        std::string userId = data["userId"];
        querySQL += w.quote(userId);

        querySQL += " AND finished=false";
        querySQL += " LIMIT 1";
      
 
        pqxx::result R = w.exec(querySQL);

      
        for (auto const &row : R) {
          resp["attemptId"] = row["id"].as<int>();
        }
        
      
        w.commit();

        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/stop_attempt")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        std::cout << "///////"<<std::endl;

        int testId = -1;
        json Checked = json::array();
        json Variants = json::array();
        json QuestionsId = json::array();
                std::cout << "///////"<<std::endl;
        {
        std::string querySQL = "SELECT * FROM attempts WHERE id=";
        std::string id = data["attemptId"].dump(); 
        querySQL += id;

        querySQL += " LIMIT 1";
        pqxx::result R = w.exec(querySQL);

        for (auto const &row : R) {
          testId = row["testid"].as<int>();
          Checked = json::parse(row["data"].as<std::string>());
          Variants = json::parse(row["lastsid"].as<std::string>());
        }
        
        }
        if(testId == -1){
            return crow::response(crow::status::BAD_REQUEST);  
        }

        std::cout << "///////"<<std::endl;

        {
        std::string querySQL = "SELECT * FROM tests WHERE id=";
        std::string id = std::to_string(testId); 
        querySQL += id;

        querySQL += " LIMIT 1";
        pqxx::result R = w.exec(querySQL);
        std::cout << "///////"<<std::endl;
        for (auto const &row : R) {
          QuestionsId = json::parse(row["data"].as<std::string>())["questionsId"] ;
        }

        if( QuestionsId.size() == 0){
            return crow::response(crow::status::BAD_REQUEST);  
        }
        }

        json questions = json::array();

                std::cout << "///////"<<std::endl;

        for (size_t i = 0; i < QuestionsId.size(); i++)
        {
            std::string querySQL = "SELECT * FROM questions WHERE id=";
            std::string id = QuestionsId[i].dump(); 
            querySQL += id;

            querySQL += " LIMIT 1";
            pqxx::result R = w.exec(querySQL);

            for (auto const &row : R) {
               questions.push_back( json::parse(row["data"].as<std::string>()) );
            }
        }

        std::cout << "///////"<<std::endl;
        if( Variants.size() != Checked.size() || Variants.size() != questions.size() || QuestionsId.size() != questions.size() ){
            return crow::response(crow::status::BAD_REQUEST);  
        }

        long long result = 0;
        for (size_t j = 0; j < questions.size(); j++)
        {
            if(questions[j]["var"].size() > Variants[j] && questions[j]["var"][std::stoi(Variants[j].dump())]["answer"] == Checked[j]){
              result+=1;
            }
        }
        std::cout << "///////"<<std::endl;

        int id = DBManager::StopAttempt(data["attemptId"], result, w);
      
        w.commit();
        json resp;
        resp["ok"] = true;
        return crow::response{ resp.dump()};
     });


 CROW_ROUTE(app, "/test/get_attempts_t_u")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        std::string querySQL = "SELECT * FROM attempts WHERE testid=";
        std::string testId = data["testId"].dump(); 
        querySQL += testId;

        querySQL += " AND userid=";
        std::string userId = data["userId"];
        querySQL += w.quote(userId);
      
 
        pqxx::result R = w.exec(querySQL);

    
        long long i = 0;
        for (auto const &row : R) {
          resp[i]["id"] = row["id"].as<int>();
          resp[i]["finished"] = row["finished"].as<bool>();
          resp[i]["result"] = row["result"].as<int>();
          resp[i]["data"] = json::parse(row["data"].as<std::string>());
          resp[i]["lastsId"] = json::parse(row["lastsId"].as<std::string>());
          i++;
        }
        
      
        w.commit();

        return crow::response{ resp.dump()};
     });

CROW_ROUTE(app, "/test/get_attempts_t")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        std::string querySQL = "SELECT * FROM attempts WHERE testid=";
        std::string testId = data["testId"].dump(); 
        querySQL += testId;
      
        pqxx::result R = w.exec(querySQL);

        long long i = 0;
        for (auto const &row : R) {
          resp[i]["id"] = row["id"].as<int>();
          resp[i]["finished"] = row["finished"].as<bool>();
          resp[i]["result"] = row["result"].as<int>();
          resp[i]["data"] = json::parse(row["data"].as<std::string>());
          resp[i]["lastsId"] = json::parse(row["lastsId"].as<std::string>());
          resp[i]["userId"] = row["userId"].as<std::string>();
          i++;
        }
        
      
        w.commit();

        return crow::response{ resp.dump()};
     });





 CROW_ROUTE(app, "/test/add_question")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        
        int id = DBManager::AddQuestionRow(data["subjectId"], data["data"], w);
      
        w.commit();
        json resp;
        resp["questionId"] = id;
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/get_questions")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();
        for (size_t i = 0; i < data["questionsId"].size(); i++)
        {
            std::string querySQL = "SELECT * FROM questions WHERE id=";
            std::string id = data["questionsId"][i].dump(); 
            querySQL += id;

            querySQL += " LIMIT 1";
            pqxx::result R = w.exec(querySQL);

            for (auto const &row : R) {
              resp.push_back( json::parse(row["data"].as<std::string>()) );
              resp[i]["id"] = std::stoi(id);
              resp[i]["subjectId"] = row["subjectid"].as<int>();
              
            }
        }
      
        w.commit();
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/update_question")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);
        
        json change;

        std::string querySQL = "SELECT * FROM questions WHERE id=";
        std::string id = data["questionId"].dump(); 
        querySQL += id;

        querySQL += " LIMIT 1";
        pqxx::result R = w.exec(querySQL);

        for (auto const &row : R) {
          change = json::parse(row["data"].as<std::string>());
        }
        if(change.empty()){
          return crow::response(crow::status::BAD_REQUEST);  
        }
        
        change["var"].push_back( data["data"]);

        int result = DBManager::UpdateQuestionRow( data["questionId"],change,w);
        w.commit();

        json resp;
        resp["ok"] = true;
        return crow::response{ resp.dump()};
     });

 CROW_ROUTE(app, "/test/get_questions_s")
     .methods("POST"_method)
     ([&](const crow::request& req) {
        auto x = crow::json::load(req.body);
        json data = json::parse(req.body);
        if (!x)
            return crow::response(crow::status::BAD_REQUEST);  
        pqxx::work w(c);


        json resp = json::array();


        std::string querySQL = "SELECT * FROM questions WHERE subjectId=";
        std::string id = data["subjectId"].dump(); 
        querySQL += id;

        pqxx::result R = w.exec(querySQL);
        std::cout << querySQL << std::endl;
        long long i = 0;
        for (auto const &row : R) {
          resp.push_back( json::parse(row["data"].as<std::string>()) );
          resp[i]["id"] =  row["id"].as<int>();
          resp[i]["subjectId"] = row["subjectid"].as<int>();
          i++;
        }
        
    
        w.commit();
        return crow::response{ resp.dump()};
     });





 app.port(4308).run();

    //get
    //pqxx::row r = w.exec1("SELECT * FROM test WHERE id=1 LIMIT 1");
    //auto one = r[1].as<std::string>();
    //std::cout << one << std::endl;


    //create
    //std::string name = "{'title':'one'}";
    //w.exec("INSERT INTO test (data) VALUES (" + w.quote(name) + ")");

    //edit
    //w.exec("UPDATE test SET data = 'newvalue' WHERE id = 1");

  
  }
  catch (std::exception const &e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}