using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Text.RegularExpressions;

namespace wpf_ndr
{
    class GetLeftDays
    {
        private HttpWebRequest req = null;
        private String username = "";
        private String password = "";
        private String url = "http://e.neusoft.edu.cn/www/signin";
        private String text = "";
        private String cookie = "";
        public GetLeftDays(String username, String password)
        {
            this.username = username;
            this.password = password;
            req = WebRequest.Create(url) as HttpWebRequest;
        }
        public void Login()
        {
            req.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Safari/537.36";
            req.Method = "GET";
            req.AllowAutoRedirect = false;
            req.ContentType = "application/x-www-form-urlencoded";
            WebResponse resp = req.GetResponse();
            Stream s = resp.GetResponseStream();
            StreamReader sr = new StreamReader(s, Encoding.Default);
            String url_page = sr.ReadToEnd();
            url_page = Regex.Match(url_page, "URL=.+\"").ToString();
            String url = url_page.Substring(4, url_page.Length - 5);
            req = WebRequest.Create(url) as HttpWebRequest;
            req.Method = "GET";
            req.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Safari/537.36";
            resp = req.GetResponse();
            s = resp.GetResponseStream();
            string cookies = resp.Headers.Get("Set-Cookie");
            Console.WriteLine(cookies);
            sr = new StreamReader(s);
            req = WebRequest.Create(url) as HttpWebRequest;
            req.Method = "POST";
            req.UserAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Safari/537.36";
            req.CookieContainer = new CookieContainer();
            req.CookieContainer.SetCookies(req.RequestUri, cookies);
            Stream post_stream = req.GetRequestStream();
            byte[] post_data = Encoding.UTF8.GetBytes("\"username\":\"" + username + "\",\"password\":\"" + password + "\",\"scope\":\"get_user_account\",\"action\":\"login\"}");
            post_stream.Write(post_data, 0, post_data.Length);
            resp = req.GetResponse();
        }
    }
}
