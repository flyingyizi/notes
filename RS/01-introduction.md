#  

书籍下载地址：[Recommender Systems Handbook.pdf](http://vdisk.weibo.com/s/uu2R8eaP7t3v1)

“Item” is the general term used to denote what the system recommends to users.
“Item”是用于表示系统向用户推荐内容的通用术语。

 based on the user’s preferences and constraints.
 基于用户喜好与约束

 In seeking to mimic this behavior, the first RSs applied algorithms to leverage recommendations produced by a community of users to deliver recommendations to an active user, i.e., a user looking for suggestions. The recommendations were for items that similar users (those with similar tastes) had liked. This approach is termed collaborative-filtering and its rationale is that if the active user agreed in the past with some users, then the other recommendations coming from these similar users should be relevant as well and of interest to the active user.

在试图模仿这种行为时，第一个RS应用算法来利用由用户社区产生的向活动用户（即寻找建议的用户）提供推荐的建议。这些建议是针对相似用户（具有相似口味的用户）喜欢的项目。这种方法称为协作过滤，其基本原理是，如果活动用户过去与一些用户达成一致，那么来自这些相似用户的其他建议也应该与活动用户相关，并且对活动用户感兴趣。

There are dedicated conferences and workshops related to the field. We refer specifically to ACM Recommender Systems (RecSys), established in 2007 and now the premier annual event in recommender technology research and applications. In addition, sessions dedicated to RSs are frequently included in the more traditional conferences in the area of data bases, information systems and adaptive systems. Among these conferences are worth mentioning ACM SIGIR Special Interest Group on Information Retrieval (SIGIR), User Modeling, Adaptation and Personalization (UMAP), and ACM’s Special Interest Group on Management Of Data (SIGMOD).
需要关注下RecSys这个每年的年会论文， [2016 RecSys 参会总结](https://www.douban.com/note/583716751/), [RecSys 2017 参会总结](https://www.douban.com/note/662840537/)


## 1.2 Recommender Systems Function

various reasons as to why service providers may want to exploit this technology:

    - •Increase the number of items sold.
    - •Sell more diverse items.
    - •Increase the user satisfaction.
    - •Increase user fidelity.
    - •Better understand what the user wants.

Herlocker et al. [25], in a paper that has become a classical reference in this field, define eleven popular tasks that a RS can assist in implementing. Some may be considered as the main or core tasks that are normally associated with a RS, i.e., to offer suggestions for items that may be useful to a user. Others might be considered as more “opportunistic” ways to exploit a RS. As a matter of fact, this task differentiation is very similar to what happens with a search engine, Its primary function is to locate documents that are relevant to the user’s information need, but it can also be used to check the importance of a Web page (looking at the position of the page in the result list of a query) or to discover the various usages of a word in a collection of documents.

- •Find Some Good Items:
 
    Recommend to a user some items as a ranked list along with predictions of how much the user would like them (e.g., on a one- to five-star scale). This is the main recommendation task that many commercial systems address (see, for instance, Chapter 9). Some systems do not show the predicted rating.

- •Find all good items:

    Recommend all the items that can satisfy some user needs. In such cases it is insufficient to just find some good items. This is especially true when the number of items is relatively small or when the RS is mission-critical, such as in medical or financial applications. In these situations, in addition to the benefit derived from carefully examining all the possibilities, the user may also benefit from the RS ranking of these items or from additional explanations that the RS generates.

- •Annotation in context:

    Given an existing context, e.g., a list of items, emphasize some of them depending on the user’s long-term preferences. For example, a TV recommender system might annotate which TV shows displayed in the electronic program guide (EPG) are worth watching (Chapter 18 provides interesting examples of this task).

- •Recommend a sequence:

    Instead of focusing on the generation of a single recommendation, the idea is to recommend a sequence of items that is pleasing as a whole. Typical examples include recommending a TV series; a book on RSs after having recommended a book on data mining; or a compilation of musical tracks [99], [39].

- •Recommend a bundle:

    Suggest a group of items that fits well together. For instance a travel plan may be composed of various attractions, destinations, and accommodation services that are located in a delimited area. From the point of view of the user these various alternatives can be considered and selected as a single travel destination [87].

- •Just browsing:
    In this task, the user browses the catalog without any imminent intention of purchasing an item. The task of the recommender is to help the user to browse the items that are more likely to fall within the scope of the user’s interests for that specific browsing session. This is a task that has been also supported by adaptive hypermedia techniques [23].

- •Find credible recommender:

    Some users do not trust recommender systems thus they play with them to see how good they are in making recommendations. Hence, some system may also offer specific functions to let the users test its behavior in addition to those just required for obtaining recommendations.

- •Improve the profile:

    This relates to the capability of the user to provide (input) information to the recommender system about what he likes and dislikes. This is a fundamental task that is strictly necessary to provide personalized recommendations. If the system has no specific knowledge about the active user then it can only provide him with the same recommendations that would be delivered to an “average” user.

- •Express self:

    Some users may not care about the recommendations at all. Rather, what it is important to them is that they be allowed to contribute with their ratings and express their opinions and beliefs. The user satisfaction for that activity can still act as a leverage for holding the user tightly to the application (as we mentioned above in discussing the service provider’s motivations).

- •Help others:

    Some users are happy to contribute with information, e.g., their evaluation of items (ratings), because they believe that the community benefits from their contribution. This could be a major motivation for entering information into a recommender system that is not used routinely. For instance, with a car RS, a user, who has already bought her new car is aware that the rating entered in the system is more likely to be useful for other users rather than for the next time she will buy a car.

- •Influence others:

    In Web-based RSs, there are users whose main goal is to ex-plicitly influence other users into purchasing particular products. As a matter of fact, there are also some malicious users that may use the system just to promote or penalize certain items (see Chapter 25).

## 1.3 Data and Knowledge Sources

- Items.

    Items are the objects that are recommended. Items may be characterized by their complexity and their value or utility.

- Users.
    Users of a RS, as mentioned above, may have very diverse goals and characteristics

- Transactions.

    We generically refer to a transaction as a recorded interaction between a user and the RS. Transactions are log-like data that store important information generated during the human-computer interaction and which are useful for the recommendation generation algorithm that the system is using. 

## 1.4 Recommendation Techniques

In order to implement its core function, identifying the useful items for the user, a RS must predict that an item is worth recommending. I

This view of the core recommendation computation as the prediction of the utility of an item for a user has been suggested in [3]. They model this degree of utility of the user $u$ for the item $i$ as a (real valued) function $R(u,i)$, as is normally done in collaborative filtering by considering the ratings of users for items. Then the fundamental task of a collaborative filtering RS is to predict the value of $R$ over pairs of users and items, i.e., to compute $\hat{R}(u,i)$, where we denote with $\hat{R}$ the estimation, computed by the RS, of the true function $R$. Consequently, having computed this prediction for the active user $u$ on a set of items, i.e., $\hat{R}(u,i_1)$,..., $\hat{R}(u,i_N)$ the system will recommend the items $i_{j_1},...,i_{j_k} (K≤N)$ with the largest predicted utility. $K$ is typically a small number, i.e., much smaller than the cardinality of the item data set or the items on which a user utility prediction can be computed, i.e., RSs “filter” the items that are recommended to users.

25] distinguishes between six different classes of recommendation approaches:

- Content-based 基于内容:

    The system learns to recommend items that are similar to the ones that the user liked in the past. The similarity of items is calculated based on the features associated with the compared items. For example, if a user has positively rated a movie that belongs to the comedy genre, then the system can learn to recommend other movies from this genre. Chapter 3 provides an overview of contentbased recommender systems, imposing some order among the extensive and diverse aspects involved in their design and implementation. It presents the basic concepts and terminology of content-based RSs, their high level architecture, and their main advantages and drawbacks. The chapter then surveys state-of-the-art systems that have been adopted in several application domains. The survey encompasses a thorough description of both classical and advanced techniques for representing items and user profiles. Finally, it discusses trends and future research which might lead towards the next generation of recommender systems.

- Collaborative filtering 基于协同过滤:

    The simplest and original implementation of this approach [93] recommends to the active user the items that other users with similar tastes liked in the past. The similarity in taste of two users is calculated  based on the similarity in the rating history of the users. This is the reason why [94] refers to collaborative filtering as “people-to-people correlation.” Collaborative filtering is considered to be the most popular and widely implemented technique in RS.
    Chapter 4 presents a comprehensive survey of neighborhood-based methods for collaborative filterin
    Chapter 5 presents several recent extensions available for building CF recommenders.

- Demographic 基于人口:

    This type of system recommends items based on the demographic profile of the user. The assumption is that different recommendations should be generated for different demographic niches. Many Web sites adopt simple and effective personalization solutions based on demographics. For example, users are dispatched to particular Web sites based on their language or country. Or suggestions may be customized according to the age of the user. While these approaches have been quite popular in the marketing literature, there has been relatively little proper RS research into demographic systems [59].

- Knowledge-based 基于知识:

    Knowledge-based systems recommend items based on specific domain knowledge about how certain item features meet users needs and preferences and, ultimately, how the item is useful for the user. Notable knowledgebased recommender systems are case-based [22, 87]. In these systems a similarity function estimates how much the user needs (problem description) match the recommendations (solutions of the problem). Here the similarity score can be directly interpreted as the utility of the recommendation for the user.
    Constraint-based systems are another type of knowledge-based RSs (Chapter 6). In terms of used knowledge, both systems are similar:

- Community-based:

    This type of system recommends items based on the preferences of the users friends. This technique follows the epigram “Tell me who your friends are, and I will tell you who you are”. [8, 14

- Hybrid recommender systems:

    These RSs are based on the combination of the above mentioned techniques. 

## 1.5 Application and Evaluation

    对RS，要应用在什么领域从而考虑使用什么技术这个很重要。
    Chapter 11 of this handbook provides guidelines for matching the application domain to the recommendation technique. 

## (//TODO) 1.6 Recommender Systems and Human Computer Interaction

