from selenium import webdriver

# creates a new firefox sesion
def test_web_server():
    opts = webdriver.FirefoxOptions()
    opts.add_argument("--headless")
    serv = webdriver.FirefoxService( executable_path='/usr/local/bin/geckodriver' )
    driver = webdriver.Firefox( options=opts, service=serv )
    print("Testing web server")
    driver.get("http://localhost:8080")
    driver.close()


if __name__ == "__main__":
    test_web_server()
