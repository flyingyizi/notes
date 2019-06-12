
#


- Spring的DAO理念
  
    DAO(Data Access Object)是用于访问数据的对象，虽然我们在大多数情况下，将数据保存在数据库中，但这并不是唯一的选择，你也可以将数据存储到文件中或LDAP中。 DAO不但屏蔽了数据存储的最终介质的不同，也屏蔽了具体的实现技术的不同。

Spring 提供了一套抽象的DAO类，供开发者扩展，这有利于以统一的方式操作各种DAO技术，例如JDO、JDBC等，这些抽象DAO类提供了设置数据源及相关辅助信息的方法，而其中的一些方法同具体DAO技术相关。目前，Spring DAO抽象提供了以下几种类：

- JdbcDaoSupport：JDBC DAO抽象类，开发者需要为它设置数据源（DataSource），通过子类，开发者能够获得JdbcTemplate来访问数据库。  
- HibernateDaoSupport：Hibernate DAO抽象类。开发者需要为它配置Hibernate - SessionFactory。通过其子类，开发者能够获得Hibernate实现。  
- JdoDaoSupport：Spring为JDO提供的DAO抽象类，开发者需要为它配置PersistenceManagerFactory，通过其子类开发者能够获得JdoTemplate。  

Spring为各种支持的持久化技术，都提供了简单操作的模板和回调
|ORM持久化技术| 	模板类|
|--|--|
|JDBC 	     |  org.springframework.jdbc.core.JdbcTemplate |
|Hibernate5.0 |	org.springframework.orm.hibernate5.HibernateTemplate |
|IBatis(MyBatis) |	org.springframework.orm.ibatis.SqlMapClientTemplate |
|JPA |	org.springfrmaework.orm.jpa.JpaTemplate |

# JDBC API实践

Spnng提供了两种使用JDBCAPI的最佳实践，一种是以jdbcTemplate为核心的基于Template的JDBC使用方式，另一种则是在jdbcTemplate基础之上构建的基于操作对象的BC使用方式。下面让我们先从基于Template的JDBC使用方式开始看起。
## 基于Template的JDBC使用方式

为了解决JDBC API太底层访问的种种问题，spring提供了org.springframework.jdbc.core.jdbcTemplate作为数据访问的helper类。

在下面的例子中，Spring中配置如下的引用链：DataSource --> JdbcTemplate --> Dao --> Service --> Action/Servlet。 "-->"表示将左边的对象注入到右边的对象当中

https://www.cnblogs.com/qingruihappy/p/7858316.html

- 数据IoC准备

  在applicationContext.xml中进行配置
    ```xml
    <!-- IOC和DI的注解扫描 -->
    <context:component-scan base-package="com.spring" ></context:component-scan>    
    <!-- 打开AOP的注解 -->
    <!-- 这里用的是中间的横线而不是下划线 -->
    <aop:aspectj-autoproxy></aop:aspectj-autoproxy>
    <!-- dataSource和jdbcTemplate都是直接配置 -->    
    <bean id="dataSource" class="org.springframework.jdbc.datasource.DriverManagerDataSource">
        <property name="driverClassName"><value>com.mysql.jdbc.Driver</value></property>
        <property name="url"><value>jdbc:mysql://localhost:3306/myspringjdbcdb</value></property>
        <property name="username"><value>root</value></property>
        <property name="password"><value>root</value></property>
    </bean>
    <bean id="jdbcTemplate" class="org.springframework.jdbc.core.JdbcTemplate">
        <property name="dataSource"><ref local="dataSource" /></property>
    </bean>
    <bean id="userDao" class="com.spring.dao.UserDao">
        <property name="jdbcTemplate" ref="jdbcTemplate"></property>
    </bean>
    <bean id="userService" class="com.spring.service.UserService">
        <property name="userDao" ref="userDao"></property>
    </bean>    
    ```
- UserDao.java

    ```java
    package com.spring.dao;
    import org.springframework.jdbc.core.JdbcTemplate;
    import com.spring.domain.User;
    public class UserDao {
        private JdbcTemplate jdbcTemplate;
        public JdbcTemplate getJdbcTemplate() {
            return jdbcTemplate;
        }
        public void setJdbcTemplate(JdbcTemplate jdbcTemplate) {
            this.jdbcTemplate = jdbcTemplate;
        }
        public void addUser(User user) {
            String sql = "insert into user (username, password) values (?, ?)";
            jdbcTemplate.update(sql, user.getUsername(), user.getPassword());
        }
        public void updateUser(User user) {
            String sql = "update user set username=? where username= ?";
            jdbcTemplate.update(sql,  user.getUsername() + "_new", user.getUsername());
        }
    }
    ```
- UserSerice.java

    ```java
    package com.spring.service;
    import com.spring.dao.UserDao;
    import com.spring.domain.User;
    public class UserService {
        // 加入userDao作为成员变变量
        private UserDao userDao;
        // 注意这里要增加get和set方法
        public UserDao getUserDao() {
            return userDao;
        }
        public void setUserDao(UserDao userDao) {
            this.userDao = userDao;
        }
        public void addUser(User user) {
            userDao.addUser(user);
        }
    }
    ```

- 最后使用测试
    ```java
    package com.spring.test;
    import org.junit.Test;
    import org.springframework.context.ApplicationContext;
    import org.springframework.context.support.ClassPathXmlApplicationContext;
    import com.spring.domain.User;
    import com.spring.service.UserService;
    public class TestJDBCTemplate {
        @Test
        public void test2() {
            ApplicationContext ctx = new ClassPathXmlApplicationContext("bean.xml");
            UserService userService = (UserService) ctx.getBean("userService");
            User user = new User();
            user.setPassword("111");
            user.setUsername("小王");

            userService.addUser(user);
        }
    }
    ```

#
