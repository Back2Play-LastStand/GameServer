using MySql.Data.MySqlClient;
using System.Net;
using System.Text;

class LoginServer
{
    static void Main(String[] args)
    {
        var listener = new HttpListener();
        var connector = "Server=localhost;Port=3306;Database=sunnight;Uid=root;Pwd=1234";
        listener.Prefixes.Add("http://localhost:3333/");

        listener.Start();
        while (true)
        {
            var context = listener.GetContext();

            if (context.Request.RawUrl == "/login")
            {
                using (var stream = new StreamReader(context.Request.InputStream, context.Request.ContentEncoding))
                {
                    string? line;
                    bool success = false;
                    while ((line = stream.ReadLine()) != null)
                    {
                        string[] str = line.Split('&');
                        string username = str[0];
                        string password = str[1];
                        success = CheckLogin(username, password);
                    }

                    string responseString = success ? "success" : "fail";

                    byte[] buffer = Encoding.UTF8.GetBytes(responseString);
                    context.Response.ContentLength64 = buffer.Length;
                    Stream output = context.Response.OutputStream;
                    output.Write(buffer, 0, buffer.Length);

                    output.Close();
                }
            }
        }

        bool CheckLogin(string name, string pw)
        {
            using (MySqlConnection connection = new MySqlConnection(connector)) // 새로운 연결을 매 요청마다 생성
            {
                try
                {
                    connection.Open();
                    string sql = "SELECT * FROM player";

                    MySqlCommand cmd = new MySqlCommand(sql, connection);
                    MySqlDataReader table = cmd.ExecuteReader();

                    while (table.Read())
                    {
                        //Console.WriteLine((string)table["player_name"], table["password"]);
                        string? playerName = table["player_name"].ToString();
                        // 인덱스를 사용하여 데이터 접근
                        string password = table.GetString(table.GetOrdinal("password"));
                        Console.WriteLine($"Player: {playerName}, Password: {password}");
                        if (name == playerName)
                        {
                            if (pw == password)
                            {
                                Console.WriteLine("success to login");
                                return true;
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.ToString());
                    return false;
                }
            }
            return false;
        }

    }
}