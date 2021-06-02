#include "connection_pool.h"


int main()
{
    auto conn = connection_pool::getinstance().getconnection();

    string sql = R"(select * from t_user where UserID='17368674639';)"; 
    MYSQL_RES* results = conn->query(sql);

    cout << "records count = " << results->lengths << endl;



    getchar();
}
