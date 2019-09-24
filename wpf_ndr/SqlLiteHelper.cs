using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Data.SQLite;
using System.Data;
using System.IO;

namespace wpf_ndr
{
    class UserORM
    {
        public Object id = null;
        public String username = null;
        public String password = null;
        public String domain = null;
        public Object last_login = null;

        public override string ToString()
        {
            return "id:" + id.ToString() + "\nusername:" + username + "\npassword:" + password + "\ndomain:" + domain;
        }

    }
    class SqlLiteHelper
    {
        private const string file_name = "cfg.sqlite";
        private SQLiteConnection _instance;
        private string connect_string = "Data Source=" + Utils.home_dir + file_name + ";Version=3;";
        public SqlLiteHelper()
        {
            _instance = new SQLiteConnection(connect_string);
            if (!Directory.Exists(Utils.home_dir))
            {
                Directory.CreateDirectory(Utils.home_dir);
            }
            if (!File.Exists(Utils.home_dir + file_name))
            {
                SQLiteConnection.CreateFile(Utils.home_dir + file_name);
                string sql = "create table if not exists up(" +
                    "id integer primary key," +
                    "un varchar(30) not null," +
                    "ps varchar(30) not null," +
                    "domain varchar(10) not null," +
                    "last int null" +
                    ")";
                this.Open();
                SQLiteCommand command = new SQLiteCommand(sql, _instance);
                command.ExecuteNonQuery();
                this.Close();
            }
        }
        public void Open()
        {
            _instance.Open();
        }
        public void Close()
        {
            _instance.Close();
        }
        public UserORM GetLastLogin()
        {
            UserORM res = new UserORM();
            string sql = "select * from up where last = 1";
            Open();
            SQLiteCommand command = new SQLiteCommand(sql, _instance);
            SQLiteDataReader reader = command.ExecuteReader();
            reader.Read();
            res.id = reader["id"];
            res.username = reader["un"].ToString();
            res.password = reader["ps"].ToString();
            res.domain = reader["domain"].ToString();
            res.last_login = reader["last"];
            reader.Close();
            Close();
            return res;
        }
        public void AddUser(UserORM o)
        {
            string sql = "select count(id) as c from up where un=@un";
            Open();
            SQLiteCommand command = new SQLiteCommand(sql, _instance);
            command.Parameters.Add("@un", DbType.String);
            command.Parameters["@un"].Value = o.username;
            SQLiteDataReader reader = command.ExecuteReader();
            reader.Read();
            int count = Convert.ToInt32(reader["c"].ToString());
            reader.Close();
            Close();
            if (count == 0)
            {
                sql = "insert into up(id,un,ps,domain,last) values(@id,@un,@ps,@domain,@last)";
                Open();
                command = new SQLiteCommand(sql, _instance);
                command.Parameters.Add("@id", DbType.Int32);
                command.Parameters.Add("@un", DbType.String);
                command.Parameters.Add("@ps", DbType.String);
                command.Parameters.Add("@domain", DbType.String);
                command.Parameters.Add("@last", DbType.Int32);

                command.Parameters["@id"].Value = o.id;
                command.Parameters["@un"].Value = o.username;
                command.Parameters["@ps"].Value = o.password;
                command.Parameters["@domain"].Value = o.domain;
                command.Parameters["@last"].Value = o.last_login;
                command.ExecuteNonQuery();
                Close();
            }
            else
            {
                sql = "update up set last=0 where last=1";
                Open();
                command = new SQLiteCommand(sql, _instance);
                command.ExecuteNonQuery();
                Close();
                sql = "update up set last=1 where id=@id";
                Open();
                command = new SQLiteCommand(sql, _instance);
                command.Parameters.Add("@id", DbType.Int32);
                command.Parameters["@id"].Value = o.id;
                Close();
            }
        }
    }
}
