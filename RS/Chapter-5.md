
# Chapter 5 Advances in Collaborative Filtering Yehuda Koren and Robert Bell
Abstract
The collaborative filtering (CF) approach to recommenders has recently enjoyed
much interest and progress. The fact that it played a central role within the recently
completedNetflixcompetitionhascontributedtoitspopularity.Thischaptersurveys
the recent progress in the field. Matrix factorization techniques, which became a first
choice for implementing CF, are described together with recent innovations. We
also describe several extensions that bring competitive accuracy into neighborhood
methods, which used to dominate the field. The chapter demonstrates how to utilize
temporal models and implicit feedback to extend models accuracy. In passing, we
include detailed descriptions of some the central methods developed for tackling the
challenge of the Netflix Prize competition.
5.1 Introduction
Collaborative filtering (CF) methods produce user specific recommendations of
items based on patterns of ratings or usage (e.g., purchases) without need for ex-
Yehuda Koren

Yahoo! Research, e-mail: yehuda@yahoo-inc.com
Robert Bell
AT&T Labs – Research e-mail: rbell@research.att.com
This article includes copyrighted materials, which were reproduced with permission of ACM and
IEEE. The original articles are:
R. Bell and Y. Koren, “Scalable Collaborative Filtering with Jointly Derived Neighborhood
Interpolation Weights”, IEEE International Conference on Data Mining (ICDM’07), pp. 43–52,
c ⃝ 2007 IEEE. Reprinted by permission.
Y. Koren, “Factorization Meets the Neighborhood: a Multifaceted Collaborative Filtering
Model”, Proc. 14th ACM SIGKDD International Conference on Knowledge Discovery and Data
Mining, c ⃝ 2008 ACM, Inc. Reprinted by permission. http://doi.acm.org/10.1145/
1401890.1401944
F. Ricci et al. (eds.), Recommender Systems Handbook,
DOI 10.1007/978-0-387-85820-3_5, © Springer Science+Business Media, LLC 2011
145
146 Yehuda Koren and Robert Bell
ogenous information about either items or users. While well established methods
work adequately for many purposes, we present several recent extensions available
to analysts who are looking for the best possible recommendations.
The Netflix Prize competition that began in October 2006 has fueled much re-
cent progress in the field of collaborative filtering. For the first time, the research
community gained access to a large-scale, industrial strength data set of 100 million
movie ratings—attracting thousands of scientists, students, engineers and enthusi-
asts to the field. The nature of the competition has encouraged rapid development,
where innovators built on each generation of techniques to improve prediction accu-
racy. Because all methods are judged by the same rigid yardstick on common data,
the evolution of more powerful models has been especially efficient.
Recommender systems rely on various types of input. Most convenient is high
quality explicit feedback, where users directly report on their interest in products.
For example, Netflix collects star ratings for movies and TiVo users indicate their
preferences for TV shows by hitting thumbs-up/down buttons.
Because explicit feedback is not always available, some recommenders infer user
preferences from the more abundant implicit feedback, which indirectly reflects
opinion through observing user behavior [22]. Types of implicit feedback include
purchase history, browsing history, search patterns, or even mouse movements. For
example, a user who purchased many books by the same author probably likes that
author. This chapter focuses on models suitable for explicit feedback. Nonetheless,
we recognize the importance of implicit feedback, an especially valuable informa-
tion source for users who do not provide much explicit feedback. Hence, we show
how to address implicit feedback within the models as a secondary source of infor-
mation.
In order to establish recommendations, CF systems need to relate two funda-
mentally different entities: items and users. There are two primary approaches to
facilitate such a comparison, which constitute the two main techniques of CF: the
neighborhood approach and latent factor models. Neighborhood methods focus on
relationships between items or, alternatively, between users. An item-item approach
models the preference of a user to an item based on ratings of similar items by the
same user. Latent factor models, such as matrix factorization (aka, SVD), comprise
an alternative approach by transforming both items and users to the same latent fac-
tor space. The latent space tries to explain ratings by characterizing both products
and users on factors automatically inferred from user feedback.
Producing more accurate prediction methods requires deepening their founda-
tions and reducing reliance on arbitrary decisions. In this chapter, we describe a
variety of recent improvements to the primary CF modeling techniques. Yet, the
quest for more accurate models goes beyond this. At least as important is the identi-
fication of all the signals, or features, available in the data. Conventional techniques
address the sparse data of user-item ratings. Accuracy significantly improves by also
utilising other sources of information. One prime example includes all kinds of tem-
Y. Koren. “Collaborative Filtering with Temporal Dynamics.” Proc. 15th ACM SIGKDD Inter-
national Conference on Knowledge Discovery and Data Mining, pp. 447–456, c ⃝ 2009 ACM, Inc.
Reprinted by permission. http://doi.acm.org/10.1145/1557019.1557072
5 Advances in Collaborative Filtering 147
poral effects reflecting the dynamic, time-drifting nature of user-item interactions.
No less important is listening to hidden feedback such as which items users chose to
rate (regardless of rating values). Rated items are not selected at random, but rather
reveal interesting aspects of user preferences, going beyond the numerical values of
the ratings.
Section 5.3 surveys matrix factorization techniques, which combine implementa-
tion convenience with a relatively high accuracy. This has made them the preferred
technique for addressing the largest publicly available dataset - the Netflix data.
This section describes the theory and practical details behind those techniques. In
addition, much of the strength of matrix factorization models stems from their nat-
ural ability to handle additional features of the data, including implicit feedback
and temporal information. This section describes in detail how to enhance matrix
factorization models to address such features.
Section 5.4 turns attention to neighborhood methods. The basic methods in this
family are well known, and to a large extent are based on heuristics. Some re-
cently proposed techniques address shortcomings of neighborhood techniques by
suggesting more rigorous formulations, thereby improving prediction accuracy. We
continue at Section 5.5 with a more advanced method, which uses the insights of
common neighborhood methods, with global optimization techniques typical of fac-
torization models. This method allows lifting the limit on neighborhood size, and
also addressing implicit feedback and temporal dynamics. The resulting accuracy
is close to that of matrix factorization models, while offering some practical advan-
tages.
Pushing the foundations of the models to their limits reveals surprising links
among seemingly unrelated techniques. We elaborate on this in Section 5.6 to show
that, at their limits, user-user and item-item neighborhood models may converge
to a single model. Furthermore, at that point, both become equivalent to a simple
matrix factorization model. The connections reduce the relevance of some previous
distinctions such as the traditional broad categorization of matrix factorization as
“model based” and neighborhood models as “memory based”.
5.2 Preliminaries
We are given ratings for m users (aka customers) and n items (aka products). We
reserve special indexing letters to distinguish users from items: for users u,v, and
for items i, j,l. A rating r ui indicates the preference by user u of item i, where high
values mean stronger preference. For example, values can be integers ranging from
1 (star) indicating no interest to 5 (stars) indicating a strong interest. We distinguish
predicted ratings from known ones, by using the notation ˆ r ui for the predicted value
of r ui .
The scalar t ui denotes the time of rating r ui . One can use different time units,
based on what is appropriate for the application at hand. For example, when time
is measured in days, then t ui counts the number of days elapsed since some early
148 Yehuda Koren and Robert Bell
time point. Usually the vast majority of ratings are unknown. For example, in the
Netflix data 99% of the possible ratings are missing because a user typically rates
only a small portion of the movies. The (u,i) pairs for which r ui is known are stored
in the set K = {(u,i) | r ui is known}. Each user u is associated with a set of items
denoted by R(u), which contains all the items for which ratings by u are available.
Likewise, R(i) denotes the set of users who rated item i. Sometimes, we also use
a set denoted by N(u), which contains all items for which u provided an implicit
preference (items that he rented/purchased/watched, etc.).
Models for the rating data are learnt by fitting the previously observed ratings.
However, our goal is to generalize those in a way that allows us to predict future,
unknownratings.Thus,cautionshouldbeexercisedtoavoidoverfittingtheobserved
data. We achieve this by regularizing the learnt parameters, whose magnitudes are
penalized. Regularization is controlled by constants which are denoted as: λ 1 , λ 2 ,...
Exact values of these constants are determined by cross validation. As they grow,
regularization becomes heavier.
5.2.1 Baseline predictors
CF models try to capture the interactions between users and items that produce
the different rating values. However, much of the observed rating values are due to
effects associated with either users or items, independently of their interaction. A
principal example is that typical CF data exhibit large user and item biases – i.e.,
systematic tendencies for some users to give higher ratings than others, and for some
items to receive higher ratings than others.
We will encapsulate those effects, which do not involve user-item interaction,
within the baseline predictors (also known as biases). Because these predictors tend
to capture much of the observed signal, it is vital to model them accurately. Such
modeling enables isolating the part of the signal that truly represents user-item in-
teraction, and subjecting it to more appropriate user preference models.
Denote by µ the overall average rating. A baseline prediction for an unknown
rating r ui is denoted by b ui and accounts for the user and item effects:
b ui = µ +b u +b i (5.1)
The parameters b u and b i indicate the observed deviations of user u and item i, re-
spectively, from the average. For example, suppose that we want a baseline predictor
for the rating of the movie Titanic by user Joe. Now, say that the average rating over
all movies, µ , is 3.7 stars. Furthermore, Titanic is better than an average movie, so
it tends to be rated 0.5 stars above the average. On the other hand, Joe is a critical
user, who tends to rate 0.3 stars lower than the average. Thus, the baseline predictor
for Titanic’s rating by Joe would be 3.9 stars by calculating 3.7−0.3+0.5. In order
to estimate b u and b i one can solve the least squares problem
5 Advances in Collaborative Filtering 149
min
b ∗
∑
(u,i)∈ K
(r ui − µ −b u −b i ) 2 + λ 1 ( ∑
u
b 2
u + ∑
i
b 2
i ).
Here, the first term ∑ (u,i)∈ K (r ui − µ +b u +b i ) 2 strives to find b u ’s and b i ’s that fit
the given ratings. The regularizing term – λ 1
(∑
u b 2 u
+∑
i b 2 i ) – avoids overfitting
by penalizing the magnitudes of the parameters. This least square problem can be
solved fairly efficiently by the method of stochastic gradient descent (described in
Subsection 5.3.1).
For the Netflix data the mean rating ( µ ) is 3.6. As for the learned user biases (b u ),
their average is 0.044 with standard deviation of 0.41. The average of their absolute
values (|b u |) is: 0.32. The learned item biases (b i ) average to -0.26 with a standard
deviation of 0.48. The average of their absolute values (|b i |) is 0.43.
An easier, yet somewhat less accurate way to estimate the parameters is by de-
coupling the calculation of the b i ’s from the calculation of the b u ’s. First, for each
item i we set
b i =
∑ u∈R(i) (r ui − µ )
λ 2 +|R(i)|
.
Then, for each user u we set
b u =
∑ i∈R(u) (r ui − µ −b i )
λ 3 +|R(u)|
.
Averages are shrunk towards zero by using the regularization parameters, λ 2 , λ 3 ,
which are determined by cross validation. Typical values on the Netflix dataset are:
λ 2 = 25, λ 3 = 10.
In Subsection 5.3.3.1, we show how the baseline predictors can be improved by
also considering temporal dynamics within the data.
5.2.2 The Netflix data
In order to compare the relative accuracy of algorithms described in this chapter,
we evaluated all of them on the Netflix data of more than 100 million date-stamped
movieratingsperformedbyanonymousNetflixcustomersbetweenNovember,1999
and December 2005 [5]. Ratings are integers ranging between 1 and 5. The data
spans 17,770 movies rated by over 480,000 users. Thus, on average, a movie re-
ceives 5600 ratings, while a user rates 208 movies, with substantial variation around
each of these averages. To maintain compatibility with results published by others,
we adopt some standards that were set by Netflix. First, quality of the results is
usually measured by the root mean squared error (RMSE):
√
∑
(u,i)∈TestSet
(r ui − ˆ r ui ) 2 /|TestSet|
150 Yehuda Koren and Robert Bell
a measure that puts more emphasis on large errors compared with the alternative of
mean absolute error. (Consider Chapter 8 for a comprehensive survey of alternative
evaluation metrics of recommender systems.)
We report results on a test set provided by Netflix (also known as the Quiz set),
which contains over 1.4 million recent ratings. Compared with the training data, the
test set contains many more ratings by users that do not rate much and are therefore
harder to predict. In a way, this represents real requirements for a CF system, which
needs to predict new ratings from older ones, and to equally address all users, not
just the heavy raters.
The Netflix data is part of the Netflix Prize competition, where the benchmark
is Netflix’s proprietary system, Cinematch, which achieved a RMSE of 0.9514 on
the test set. The grand prize was awarded to a team that managed to drive this
RMSE below 0.8563 (10% improvement) after almost three years of extensive ef-
forts. Achievable RMSE values on the test set lie in a quite compressed range, as
evident by the difficulty to win the grand prize. Nonetheless, there is evidence that
small improvements in RMSE terms can have a significant impact on the quality of
the top few presented recommendations [17, 19].
5.2.3 Implicit feedback
This chapter is centered on explicit user feedback. Nonetheless, when additional
sources of implicit feedback are available, they can be exploited for better under-
standing user behavior. This helps to combat data sparseness and can be particularly
helpful for users with few explicit ratings. We describe extensions for some of the
models to address implicit feedback.
For a dataset such as the Netflix data, the most natural choice for implicit feed-
back would probably be movie rental history, which tells us about user preferences
without requiring them to explicitly provide their ratings. For other datasets, brows-
ing or purchase history could be used as implicit feedback. However, such data is
not available to us for experimentation. Nonetheless, a less obvious kind of implicit
data does exist within the Netflix dataset. The dataset does not only tell us the rating
values, but also which movies users rate, regardless of how they rated these movies.
In other words, a user implicitly tells us about her preferences by choosing to voice
her opinion and vote a (high or low) rating. This creates a binary matrix, where
“1” stands for “rated”, and “0” for “not rated”. While this binary data may not be
as informative as other independent sources of implicit feedback, incorporating this
kind of implicit data does significantly improves prediction accuracy. The benefit
of using the binary data is closely related to the fact that ratings are not missing at
random; users deliberately choose which items to rate (see Marlin et al. [21]).
5 Advances in Collaborative Filtering 151
5.3 Matrix factorization models
Latent factor models approach collaborative filtering with the holistic goal to un-
cover latent features that explain observed ratings; examples include pLSA [15],
neural networks [24], Latent Dirichlet Allocation [7], and models that are induced
by factorization of the user-item ratings matrix (also known as SVD-based mod-
els). Recently, matrix factorization models have gained popularity, thanks to their
attractive accuracy and scalability.
In information retrieval, SVD is well established for identifying latent semantic
factors [9]. However, applying SVD to explicit ratings in the CF domain raises dif-
ficulties due to the high portion of missing values. Conventional SVD is undefined
when knowledge about the matrix is incomplete. Moreover, carelessly addressing
only the relatively few known entries is highly prone to overfitting. Earlier works
relied on imputation [16, 26], which fills in missing ratings and makes the rating ma-
trix dense. However, imputation can be very expensive as it significantly increases
the amount of data. In addition, the data may be considerably distorted due to in-
accurate imputation. Hence, more recent works [4, 6, 10, 17, 23, 24, 28] suggested
modeling directly only the observed ratings, while avoiding overfitting through an
adequate regularized model.
In this section we describe several matrix factorization techniques, with increas-
ing complexity and accuracy. We start with the basic model – “SVD”. Then, we
show how to integrate other sources of user feedback in order to increase prediction
accuracy, through the “SVD++ model”. Finally we deal with the fact that customer
preferences for products may drift over time. Product perception and popularity are
constantly changing as new selection emerges. Similarly, customer inclinations are
evolving, leading them to ever redefine their taste. This leads to a factor model that
addresses temporal dynamics for better tracking user behavior.
5.3.1 SVD
Matrix factorization models map both users and items to a joint latent factor space
of dimensionality f, such that user-item interactions are modeled as inner products
in that space. The latent space tries to explain ratings by characterizing both prod-
ucts and users on factors automatically inferred from user feedback. For example,
when the products are movies, factors might measure obvious dimensions such as
comedy vs. drama, amount of action, or orientation to children; less well defined
dimensions such as depth of character development or “quirkiness”; or completely
uninterpretable dimensions.
Accordingly, each item i is associated with a vector q i ∈ R f , and each user u is
associated with a vector p u ∈ R f . For a given item i, the elements of q i measure the
extent to which the item possesses those factors, positive or negative. For a given
user u, the elements of p u measure the extent of interest the user has in items that
are high on the corresponding factors (again, these may be positive or negative).
152 Yehuda Koren and Robert Bell
The resulting dot product, 1 q T
i
p u , captures the interaction between user u and item
i—i.e., the overall interest of the user in characteristics of the item. The final rating
is created by also adding in the aforementioned baseline predictors that depend only
on the user or item. Thus, a rating is predicted by the rule
ˆ r ui = µ +b i +b u +q T
i
p u . (5.2)
In order to learn the model parameters (b u ,b i ,p u and q i ) we minimize the regu-
larized squared error
min
b ∗ ,q ∗ ,p ∗
∑
(u,i)∈ K
(r ui − µ −b i −b u −q T
i
p u ) 2 + λ 4 (b 2
i +b 2 u +∥q i ∥ 2 +∥p u ∥ 2 ).
The constant λ 4 , which controls the extent of regularization, is usually determined
by cross validation. Minimization is typically performed by either stochastic gradi-
ent descent or alternating least squares.
Alternatingleastsquarestechniquesrotatebetweenfixingthe p u ’stosolveforthe
q i ’s and fixing the q i ’s to solve for the p u ’s. Notice that when one of these is taken as
a constant, the optimization problem is quadratic and can be optimally solved; see
[2, 4].
An easy stochastic gradient descent optimization was popularized by Funk [10]
and successfully practiced by many others [17, 23, 24, 28]. The algorithm loops
through all ratings in the training data. For each given rating r ui , a prediction (ˆ r ui )
is made, and the associated prediction error e ui
def
= r ui − ˆ r ui is computed. For a given
training case r ui , we modify the parameters by moving in the opposite direction of
the gradient, yielding:
• b u ← b u + γ ·(e ui − λ 4 ·b u )
• b i ← b i + γ ·(e ui − λ 4 ·b i )
• q i ← q i + γ ·(e ui · p u − λ 4 ·q i )
• p u ← p u + γ ·(e ui ·q i − λ 4 · p u )
When evaluating the method on the Netflix data, we used the following values for
the meta parameters: γ =0.005, λ 4 =0.02. Henceforth, we dub this method “SVD”.
A general remark is in place. One can expect better accuracy by dedicating sepa-
rate learningrates ( γ )and regularization( λ )to eachtype oflearned parameter.Thus,
for example, it is advised to employ distinct learning rates to user biases, item biases
and the factors themselves. A good, intensive use of such a strategy is described in
Takács et al. [29]. When producing exemplary results for this chapter, we did not
use such a strategy consistently, and in particular many of the given constants are
not fully tuned.
1
Recall that the dot product between two vectors x,y ∈ R f is defined as: x T y = ∑ f
k=1 x k ·y k
5 Advances in Collaborative Filtering 153
5.3.2 SVD++
Prediction accuracy is improved by considering also implicit feedback, which pro-
videsanadditionalindicationofuserpreferences.Thisisespeciallyhelpfulforthose
users that provided much more implicit feedback than explicit one. As explained
earlier, even in cases where independent implicit feedback is absent, one can cap-
ture a significant signal by accounting for which items users rate, regardless of their
rating value. This led to several methods [17, 23, 25] that modeled a user factor by
the identity of the items he/she has rated. Here we focus on the SVD++ method [17],
which was shown to offer accuracy superior to SVD.
To this end, a second set of item factors is added, relating each item i to a factor
vector y i ∈ R f . Those new item factors are used to characterize users based on the
set of items that they rated. The exact model is as follows:
ˆ r ui = µ +b i +b u +q T
i
(
p u +|R(u)| −
1
2
∑
j∈R(u)
y j
)
(5.3)
The set R(u) contains the items rated by user u.
Now, a user u is modeled as p u +|R(u)| −
1
2 ∑ j∈R(u) y j . We use a free user-factors
vector, p u , much like in (5.2), which is learnt from the given explicit ratings. This
vector is complemented by the sum |R(u)| −
1
2 ∑ j∈R(u) y j , which represents the per-
spective of implicit feedback. Since the y j ’s are centered around zero (by the reg-
ularization), the sum is normalized by |R(u)| −
1
2 , in order to stabilize its variance
across the range of observed values of |R(u)|
Model parameters are determined by minimizing the associated regularized
squared error function through stochastic gradient descent. We loop over all known
ratings in K, computing:
• b u ← b u + γ ·(e ui − λ 5 ·b u )
• b i ← b i + γ ·(e ui − λ 5 ·b i )
• q i ← q i + γ ·(e ui ·(p u +|R(u)| −
1
2 ∑ j∈R(u) y j )− λ 6 ·q i )
• p u ← p u + γ ·(e ui ·q i − λ 6 · p u )
• ∀j ∈ R(u) :
y j ← y j + γ ·(e ui ·|R(u)| −
1
2 ·q i − λ 6 ·y j )
When evaluating the method on the Netflix data, we used the following values for
the meta parameters: γ =0.007, λ 5 =0.005, λ 6 =0.015. It is beneficial to decrease
step sizes (the γ ’s) by a factor of 0.9 after each iteration. The iterative process runs
for around 30 iterations until convergence.
Several types of implicit feedback can be simultaneously introduced into the
model by using extra sets of item factors. For example, if a user u has a certain
kind of implicit preference to the items in N 1 (u) (e.g., she rented them), and a dif-
ferent type of implicit feedback to the items in N 2 (u) (e.g., she browsed them), we
could use the model
154 Yehuda Koren and Robert Bell
ˆ r ui = µ +b i +b u +q T
i

 p u +|N 1 (u)| −
1
2
∑
j∈N 1 (u)
y (1)
j
+|N 2 (u)| −
1
2
∑
j∈N 2 (u)
y (2)
j

 .
(5.4)
The relative importance of each source of implicit feedback will be automatically
learned by the algorithm by its setting of the respective values of model parameters.
5.3.3 Time-aware factor model
The matrix-factorization approach lends itself well to modeling temporal effects,
which can significantly improve its accuracy. Decomposing ratings into distinct
terms allows us to treat different temporal aspects separately. Specifically, we iden-
tify the following effects that each vary over time: (1) user biases b u (t), (2) item
biases b i (t), and (3) user preferences p u (t). On the other hand, we specify static
item characteristics, q i , because we do not expect significant temporal variation for
items, which, unlike humans, are static in nature. We start with a detailed discussion
of the temporal effects that are contained within the baseline predictors.
5.3.3.1 Time changing baseline predictors
Much of the temporal variability is included within the baseline predictors, through
two major temporal effects. The first addresses the fact that an item’s popularity
may change over time. For example, movies can go in and out of popularity as
triggered by external events such as the appearance of an actor in a new movie. This
is manifested in our models by treating the item bias b i as a function of time. The
second major temporal effect allows users to change their baseline ratings over time.
For example, a user who tended to rate an average movie “4 stars”, may now rate
such a movie “3 stars”. This may reflect several factors including a natural drift in a
user’s rating scale, the fact that ratings are given in relationship to other ratings that
were given recently and also the fact that the identity of the rater within a household
can change over time. Hence, in our models we take the parameter b u as a function
of time. This induces a template for a time sensitive baseline predictor for u’s rating
of i at day t ui :
b ui = µ +b u (t ui )+b i (t ui ) (5.5)
Here, b u (·) and b i (·) are real valued functions that change over time. The exact
way to build these functions should reflect a reasonable way to parameterize the
involving temporal changes. Our choice in the context of the movie rating dataset
demonstrates some typical considerations.
A major distinction is between temporal effects that span extended periods of
time and more transient effects. In the movie rating case, we do not expect movie
likability to fluctuate on a daily basis, but rather to change over more extended pe-
riods. On the other hand, we observe that user effects can change on a daily basis,
5 Advances in Collaborative Filtering 155
reflecting inconsistencies natural to customer behavior. This requires finer time res-
olution when modeling user-biases compared with a lower resolution that suffices
for capturing item-related time effects.
We start with our choice of time-changing item biases b i (t). We found it adequate
to split the item biases into time-based bins, using a constant item bias for each time
period. The decision of how to split the timeline into bins should balance the desire
to achieve finer resolution (hence, smaller bins) with the need for enough ratings per
bin (hence, larger bins). For the movie rating data, there is a wide variety of bin sizes
that yield about the same accuracy. In our implementation, each bin corresponds to
roughly ten consecutive weeks of data, leading to 30 bins spanning all days in the
dataset. A day t is associated with an integer Bin(t) (a number between 1 and 30 in
our data), such that the movie bias is split into a stationary part and a time changing
part
b i (t) = b i +b i,Bin(t) . (5.6)
While binning the parameters works well on the items, it is more of a challenge
on the users side. On the one hand, we would like a finer resolution for users to
detect very short lived temporal effects. On the other hand, we do not expect enough
ratings per user to produce reliable estimates for isolated bins. Different functional
forms can be considered for parameterizing temporal user behavior, with varying
complexity and accuracy.
One simple modeling choice uses a linear function to capture a possible gradual
drift of user bias. For each user u, we denote the mean date of rating by t u . Now, if u
rated a movie on day t, then the associated time deviation of this rating is defined as
dev u (t) = sign(t −t u )·|t −t u | β .
Here |t −t u | measures the number of days between dates t and t u . We set the value
of β by cross validation; in our implementation β = 0.4. We introduce a single
new parameter for each user called α u so that we get our first definition of a time-
dependent user-bias
b (1)
u (t) = b u + α u ·dev u (t).
(5.7)
This simple linear model for approximating a drifting behavior requires learning
two parameters per user: b u and α u .
A more flexible parameterization is offered by splines. Let u be a user associated
withn u ratings.Wedesignatek u timepoints–{t u
1 ,...,t u k u }–spaceduniformlyacross
the dates of u’s ratings as kernels that control the following function:
b (2)
u (t) = b u +
∑ k u
l=1 e
− σ |t−t u
l | b u
t l
∑ k u
l=1 e
− σ |t−t u
l |
(5.8)
The parameters b u
t l
are associated with the control points (or, kernels), and are auto-
matically learned from the data. This way the user bias is formed as a time-weighted
combination of those parameters. The number of control points, k u , balances flexi-
bilityandcomputationalefficiency.Inourapplicationwesetk u =n 0.25
u
,lettingitgrow
156 Yehuda Koren and Robert Bell
with the number of available ratings. The constant σ determines the smoothness of
the spline; we set σ =0.3 by cross validation.
So far we have discussed smooth functions for modeling the user bias, which
mesh well with gradual concept drift. However, in many applications there are sud-
den drifts emerging as “spikes” associated with a single day or session. For example,
in the movie rating dataset we have found that multiple ratings a user gives in a sin-
gle day, tend to concentrate around a single value. Such an effect need not span more
than a single day. The effect may reflect the mood of the user that day, the impact of
ratings given in a single day on each other, or changes in the actual rater in multi-
person accounts. To address such short lived effects, we assign a single parameter
per user and day, absorbing the day-specific variability. This parameter is denoted
by b u,t . Notice that in some applications the basic primitive time unit to work with
can be shorter or longer than a day.
In the Netflix movie rating data, a user rates on 40 different days on average.
Thus, working with b u,t requires, on average, 40 parameters to describe each user
bias. It is expected that b u,t is inadequate as a standalone for capturing the user bias,
since it misses all sorts of signals that span more than a single day. Thus, it serves
as an additive component within the previously described schemes. The time-linear
model (5.7) becomes
b (3)
u (t) = b u + α u ·dev u (t)+b u,t .
(5.9)
Similarly, the spline-based model becomes
b (4)
u (t) = b u +
∑ k u
l=1 e
− σ |t−t u
l | b u
t l
∑ k u
l=1 e
− σ |t−t u
l |
+b u,t . (5.10)
A baseline predictor on its own cannot yield personalized recommendations, as
it disregards all interactions between users and items. In a sense, it is capturing the
portion of the data that is less relevant for establishing recommendations. Nonethe-
less, to better assess the relative merits of the various choices of time-dependent
user-bias, we compare their accuracy as standalone predictors. In order to learn the
involved parameters we minimize the associated regularized squared error by using
stochastic gradient descent. For example, in our actual implementation we adopt
rule (5.9) for modeling the drifting user bias, thus arriving at the baseline predictor
b ui = µ +b u + α u ·dev u (t ui )+b u,t ui +b i +b i,Bin(t ui ) . (5.11)
To learn the involved parameters, b u , α u ,b u,t ,b i and b i,Bin(t) , one should solve
min
∑
(u,i)∈ K
(r ui − µ −b u − α u dev u (t ui )−b u,t ui −b i −b i,Bin(t ui ) ) 2
+ λ 7 (b 2
u + α 2 u +b 2 u,t ui +b 2 i +b 2 i,Bin(t ui ) ).
5 Advances in Collaborative Filtering 157
model static mov linear spline linear+ spline+
RMSE .9799 .9771 .9731 .9714 .9605 .9603
Table5.1: Comparing baseline predictors capturing main movieand user effects. As
temporal modeling becomes more accurate, prediction accuracy improves (lowering
RMSE).
Here, the first term strives to construct parameters that fit the given ratings. The
regularization term, λ 7 (b 2
u +...), avoids overfitting by penalizing the magnitudes of
the parameters, assuming a neutral 0 prior. Learning is done by a stochastic gradient
descent algorithm running 20–30 iterations, with λ 7 = 0.01.
Table 5.1 compares the ability of various suggested baseline predictors to explain
signal in the data. As usual, the amount of captured signal is measured by the root
mean squared error on the test set. As a reminder, test cases come later in time than
the training cases for the same user, so predictions often involve extrapolation in
terms of time. We code the predictors as follows:
• static, no temporal effects: b ui = µ +b u +b i .
• mov, accounting only for movie-related temporal effects: b ui = µ +b u +b i +
b i,Bin(t ui ) .
• linear , linear modeling of user biases: b ui = µ + b u + α u · dev u (t ui ) + b i +
b i,Bin(t ui ) .
• spline, spline modeling of user biases: b ui = µ +b u +
∑ k u
l=1 e
− σ |t ui −t u
l
| b u
t l
∑ k u
l=1 e
− σ |t ui −t u
l
|
+b i +
b i,Bin(t ui ) .
• linear+, linear modeling of user biases and single day effect: b ui = µ +b u + α u ·
dev u (t ui )+b u,t ui +b i +b i,Bin(t ui ) .
• spline+, spline modeling of user biases and single day effect: b ui = µ +b u +
∑ k u
l=1 e
− σ |t ui −d l | b u
t l
∑ k u
l=1 e
− σ |t ui −t u
l
|
+b u,t ui +b i +b i,Bin(t ui ) .
The table shows that while temporal movie effects reside in the data (lowering
RMSE from 0.9799 to 0.9771), the drift in user biases is much more influential.
The additional flexibility of splines at modeling user effects leads to better accuracy
compared to a linear model. However, sudden changes in user biases, which are cap-
tured by the per-day parameters, are most significant. Indeed, when including those
changes, the difference between linear modeling (“linear+”) and spline modeling
(“spline+”) virtually vanishes.
Beyond the temporal effects described so far, one can use the same methodol-
ogy to capture more effects. A primary example is capturing periodic effects. For
example, some products may be more popular in specific seasons or near certain
holidays. Similarly, different types of television or radio shows are popular through-
out different segments of the day (known as “dayparting”). Periodic effects can be
found also on the user side. As an example, a user may have different attitudes or
buying patterns during the weekend compared to the working week. A way to model
such periodic effects is to dedicate a parameter for the combinations of time periods
with items or users. This way, the item bias of (5.6), becomes
158 Yehuda Koren and Robert Bell
b i (t) = b i +b i,Bin(t) +b i,period(t) .
For example, if we try to capture the change of item bias with the season of the year,
then period(t)∈{fall,winter,spring,summer}. Similarly, recurring user effects may
be modeled by modifying (5.9) to be
b u (t) = b u + α u ·dev u (t)+b u,t +b u,period(t) .
However, we have not found periodic effects with a significant predictive power
within the movie-rating dataset, thus our reported results do not include those.
Another temporal effect within the scope of basic predictors is related to the
changing scale of user ratings. While b i (t) is a user-independent measure for the
merit of item i at time t, users tend to respond to such a measure differently. For
example, different users employ different rating scales, and a single user can change
his rating scale over time. Accordingly, the raw value of the movie bias is not com-
pletely user-independent. To address this, we add a time-dependent scaling feature
to the baseline predictors, denoted by c u (t). Thus, the baseline predictor (5.11) be-
comes
b ui = µ +b u + α u ·dev u (t ui )+b u,t ui +(b i +b i,Bin(t ui ) )·c u (t ui ). (5.12)
All discussed ways to implement b u (t) would be valid for implementing c u (t) as
well. We chose to dedicate a separate parameter per day, resulting in: c u (t) =
c u +c u,t . As usual, c u is the stable part of c u (t), whereas c u,t represents day-specific
variability. Adding the multiplicative factor c u (t) to the baseline predictor lowers
RMSE to 0.9555. Interestingly, this basic model, which captures just main effects
disregarding user-item interactions, can explain almost as much of the data variabil-
ity as the commercial Netflix Cinematch recommender system, whose published
RMSE on the same test set is 0.9514 [5].
5.3.3.2 Time changing factor model
In the previous subsection we discussed the way time affects baseline predictors.
However, as hinted earlier, temporal dynamics go beyond this, they also affect user
preferences and thereby the interaction between users and items. Users change their
preferences over time. For example, a fan of the “psychological thrillers” genre
may become a fan of “crime dramas” a year later. Similarly, humans change their
perception on certain actors and directors. This type of evolution is modeled by
taking the user factors (the vector p u ) as a function of time. Once again, we need to
model those changes at the very fine level of a daily basis, while facing the built-
in scarcity of user ratings. In fact, these temporal effects are the hardest to capture,
because preferences are not as pronounced as main effects (user-biases), but are split
over many factors.
Wemodeledeachcomponentoftheuserpreferences p u (t) T =(p u1 (t),...,p uf (t))
inthesamewaythatwetreateduserbiases.Withinthemovie-ratingdataset,wehave
5 Advances in Collaborative Filtering 159
found modeling after (5.9) effective, leading to
p uk (t) = p uk + α uk ·dev u (t)+ p uk,t k = 1,..., f . (5.13)
Here p uk captures the stationary portion of the factor, α uk ·dev u (t) approximates a
possible portion that changes linearly over time, and p uk,t absorbs the very local,
day-specific variability.
At this point, we can tie all pieces together and extend the SVD++ factor model
by incorporating the time changing parameters. The resulting model will be denoted
as timeSVD++, where the prediction rule is as follows:
ˆ r ui = µ +b i (t ui )+b u (t ui )+q T
i
(
p u (t ui )+|R(u)| −
1
2
∑
j∈R(u)
y j
)
(5.14)
The exact definitions of the time drifting parameters b i (t),b u (t) and p u (t) were
given in (5.6), (5.9) and (5.13). Learning is performed by minimizing the associ-
ated squared error function on the training set using a regularized stochastic gradi-
ent descent algorithm. The procedure is analogous to the one involving the original
SVD++ algorithm. Time complexity per iteration is still linear with the input size,
while wall clock running time is approximately doubled compared to SVD++, due
to the extra overhead required for updating the temporal parameters. Importantly,
convergence rate was not affected by the temporal parameterization, and the pro-
cess converges in around 30 iterations.
5.3.4 Comparison
In Table 5.2 we compare results of the three algorithms discussed in this sec-
tion. First is SVD, the plain matrix factorization algorithm. Second, is the SVD++
method, which improves upon SVD by incorporating a kind of implicit feedback.
Finally is timeSVD++, which accounts for temporal effects. The three methods are
compared over a range of factorization dimensions (f). All benefit from a growing
number of factor dimensions that enables them to better express complex movie-
user interactions. Note that the number of parameters in SVD++ is comparable to
their number in SVD. This is because SVD++ adds only item factors, while com-
plexity of our dataset is dominated by the much larger set of users. On the other
hand, timeSVD++ requires a significant increase in the number of parameters, be-
cause of its refined representation of each user factor. Addressing implicit feedback
by the SVD++ model leads to accuracy gains within the movie rating dataset. Yet,
the improvement delivered by timeSVD++ over SVD++ is consistently more sig-
nificant. We are not aware of any single algorithm in the literature that could deliver
such accuracy. Further evidence of the importance of capturing temporal dynamics
is the fact that a timeSVD++ model of dimension 10 is already more accurate than
160 Yehuda Koren and Robert Bell
an SVD model of dimension 200. Similarly, a timeSVD++ model of dimension 20
is enough to outperform an SVD++ model of dimension 200.
Model f=10 f=20 f=50 f=100 f=200
SVD .9140 .9074 .9046 .9025 .9009
SVD++ .9131 .9032 .8952 .8924 .8911
timeSVD++ .8971 .8891 .8824 .8805 .8799
Table 5.2: Comparison of three factor models: prediction accuracy is measured by
RMSE (lower is better) for varying factor dimensionality (f). For all models, accu-
racy improves with growing number of dimensions. SVD++ improves accuracy by
incorporatingimplicitfeedbackintotheSVDmodel.Furtheraccuracygainsareach-
ieved by also addressing the temporal dynamics in the data through the timeSVD++
model.
5.3.4.1 Predicting future days
Our models include day-specific parameters. An apparent question would be how
these models can be used for predicting ratings in the future, on new dates for which
we cannot train the day-specific parameters? The simple answer is that for those
future (untrained) dates, the day-specific parameters should take their default value.
In particular for (5.12), c u (t ui ) is set to c u , and b u,t ui is set to zero. Yet, one wonders,
if we cannot use the day-specific parameters for predicting the future, why are they
good at all? After all, prediction is interesting only when it is about the future. To
further sharpen the question, we should mention the fact that the Netflix test sets
include many ratings on dates for which we have no other rating by the same user
and hence day-specific parameters cannot be exploited.
To answer this, notice that our temporal modeling makes no attempt to capture
future changes. All it is trying to do is to capture transient temporal effects, which
had a significant influence on past user feedback. When such effects are identified
they must be tuned down, so that we can model the more enduring signal. This
allows our model to better capture the long-term characteristics of the data, while
letting dedicated parameters absorb short term fluctuations. For example, if a user
gave many higher than usual ratings on a particular single day, our models discount
those by accounting for a possible day-specific good mood, which does not reflects
the longer term behavior of this user. This way, the day-specific parameters accom-
plish a kind of data cleaning, which improves prediction of future dates.
5.3.5 Summary
In its basic form, matrix factorization characterizes both items and users by vectors
of factors inferred from patterns of item ratings. High correspondence between item
5 Advances in Collaborative Filtering 161
anduserfactorsleadstorecommendationofanitemtoauser.Thesemethodsdeliver
prediction accuracy superior to other published collaborative filtering techniques. At
the same time, they offer a memory efficient compact model, which can be trained
relatively easy. Those advantages, together with the implementation ease of gradient
based matrix factorization model (SVD), made this the method of choice within the
Netflix Prize competition.
Whatmakesthesetechniquesevenmoreconvenientistheirabilitytoaddresssev-
eral crucial aspects of the data. First, is the ability to integrate multiple forms of user
feedback. One can better predict user ratings by also observing other related actions
by the same user, such as purchase and browsing history. The proposed SVD++
model leverages multiple sorts of user feedback for improving user profiling.
Another important aspect is the temporal dynamics that make users’ tastes evolve
over time. Each user and product potentially goes through a distinct series of
changes in their characteristics. A mere decay of older instances cannot adequately
identify communal patterns of behavior in time changing data. The solution we
adopted is to model the temporal dynamics along the whole time period, allowing
us to intelligently separate transient factors from lasting ones. The inclusion of tem-
poral dynamics proved very useful in improving quality of predictions, more than
various algorithmic enhancements.
5.4 Neighborhood models
The most common approach to CF is based on neighborhood models. Chapter 4
provides an extensive survey on this approach. Its original form, which was shared
by virtually all earlier CF systems, is user-user based; see [14] for a good analy-
sis. User-user methods estimate unknown ratings based on recorded ratings of like-
minded users.
Later, an analogous item-item approach [20, 27] became popular. In those meth-
ods, a rating is estimated using known ratings made by the same user on simi-
lar items. Better scalability and improved accuracy make the item-item approach
more favorable in many cases [2, 27, 28]. In addition, item-item methods are more
amenable to explaining the reasoning behind predictions. This is because users are
familiar with items previously preferred by them, but do not know those allegedly
like-minded users. We focus mostly on item-item approaches, but the same tech-
niques can be directly applied within a user-user approach; see also Subsection
5.5.2.2.
In general, latent factor models offer high expressive ability to describe various
aspects of the data. Thus, they tend to provide more accurate results than neigh-
borhood models. However, most literature and commercial systems (e.g., those of
Amazon [20] and TiVo [1]) are based on the neighborhood models. The prevalence
of neighborhood models is partly due to their relative simplicity. However, there are
more important reasons for real life systems to stick with those models. First, they
naturally provide intuitive explanations of the reasoning behind recommendations,
162 Yehuda Koren and Robert Bell
which often enhance user experience beyond what improved accuracy may achieve.
(More on explaining recommendations appears in Chapter 15 of this book.) Sec-
ond, they can provide immediate recommendations based on newly entered user
feedback.
The structure of this section is as follows. First, we describe how to estimate the
similarity between two items, which is a basic building block of most neighborhood
techniques. Then, we move on to the widely used similarity-based neighborhood
method, which constitutes a straightforward application of the similarity weights.
We identify certain limitations of this similarity based approach. As a consequence,
in Subsection 5.4.3 we suggest a way to solve these issues, thereby improving pre-
diction accuracy at the cost of a slight increase in computation time.
5.4.1 Similarity measures
Central to most item-item approaches is a similarity measure between items. Fre-
quently, it is based on the Pearson correlation coefficient, ρ ij , which measures the
tendency of users to rate items i and j similarly. Since many ratings are unknown,
some items may share only a handful of common observed raters. The empirical
correlation coefficient, ˆ ρ ij , is based only on the common user support. It is advised
to work with residuals from the baseline predictors (the b ui ’s; see Section 5.2.1) to
compensate for user- and item-specific deviations. Thus the approximated correla-
tion coefficient is given by
ˆ ρ ij =
∑ u∈U(i,j) (r ui −b ui )(r uj −b uj )
√
∑ u∈U(i,j) (r ui −b ui ) 2
·∑
u∈U(i,j) (r uj −b uj ) 2
. (5.15)
The set U(i, j) contains the users who rated both items i and j.
Because estimated correlations based on a greater user support are more reliable,
an appropriate similarity measure, denoted by s ij , is a shrunk correlation coefficient
of the form
s ij
def
=
n ij −1
n ij −1+ λ 8
ρ ij . (5.16)
The variable n ij = |U(i, j)| denotes the number of users that rated both i and j. A
typical value for λ 8 is 100.
Such shrinkage can be motivated from a Bayesian perspective; see Section 2.6
of Gelman et al. [11]. Suppose that the true ρ ij are independent random variables
drawn from a normal distribution,
ρ ij ∼ N(0, τ 2 )
for known τ 2 . The mean of 0 is justified if the b ui account for both user and item
deviations from average. Meanwhile, suppose that
5 Advances in Collaborative Filtering 163
ˆ ρ ij | ρ ij ∼ N( ρ ij , σ 2
ij )
for known σ 2
ij . We estimate ρ ij by its posterior mean:
E( ρ ij
|ˆ
ρ ij ) =
τ 2 ˆ ρ ij
τ 2 + σ 2
ij
the empirical estimator ˆ ρ ij shrunk a fraction, σ 2
ij /( τ 2 + σ
2
ij ), of the way toward zero.
Formula (5.16) follows from approximating the varianceof a correlation by σ 2
ij =
1/(n ij −1), the value for ρ ij near 0.
Notice that the literature suggests additional alternatives for a similarity measure
[27, 28].
5.4.2 Similarity-based interpolation
Here we describe the most popular approach to neighborhood modeling, and appar-
ently also to CF in general. Our goal is to predict r ui – the unobserved rating by user
u for item i. Using the similarity measure, we identify the k items rated by u that
are most similar to i. This set of k neighbors is denoted by S k (i;u). The predicted
value of r ui is taken as a weighted average of the ratings of neighboring items, while
adjusting for user and item effects through the baseline predictors
ˆ r ui = b ui +
∑ j∈S k (i;u) s ij (r uj −b uj )
∑ j∈S k (i;u) s ij
. (5.17)
Note the dual use of the similarities for both identification of nearest neighbors and
as the interpolation weights in equation (5.17).
Sometimes, instead of relying directly on the similarity weights as interpolation
coefficients, one can achieve better results by transforming these weights. For exam-
ple, we have found at several datasets that squaring the correlation-based similarities
is helpful. This leads to a rule like: ˆ r ui = b ui +
∑ j∈S k (i;u) s 2
ij (r uj −b uj )
∑ j∈S k (i;u) s 2
ij
. Toscher et al. [31]
discuss more sophisticated transformations of these weights.
Similarity-based methods became very popular because they are intuitive and
relatively simple to implement. They also offer the following two useful properties:
1. Explainability. The importance of explaining automated recommendations is
widely recognized [13, 30]; see also Chapter 15. Users expect a system to give a
reason for its predictions, rather than presenting “black box” recommendations.
Explanations not only enrich the user experience, but also encourage users to
interact with the system, fix wrong impressions and improve long-term accu-
racy. The neighborhood framework allows identifying which of the past user
actions are most influential on the computed prediction.
164 Yehuda Koren and Robert Bell
2. New ratings. Item-item neighborhood models can provide updated recommen-
dations immediately after users enter new ratings. This includes handling new
users as soon as they provide feedback to the system, without needing to re-
train the model and estimate new parameters. This assumes that relationships
between items (the s ij values) are stable and barely change on a daily basis.
Notice that for items new to the system we do have to learn new parameters. In-
terestingly, this asymmetry between users and items meshes well with common
practices: systems need to provide immediate recommendations to new users
(or new ratings by old users) who expect quality service. On the other hand, it
is reasonable to require a waiting period before recommending items new to the
system.
However, standard neighborhood-based methods raise some concerns:
1. The similarity function (s ij ), which directly defines the interpolation weights,
is arbitrary. Various CF algorithms use somewhat different similarity measures,
trying to quantify the elusive notion of user- or item-similarity. Suppose that a
particular item is predicted perfectly by a subset of the neighbors. In that case,
we would want the predictive subset to receive all the weight, but that is impos-
sible for bounded similarity scores like the Pearson correlation coefficient.
2. Previous neighborhood-based methods do not account for interactions among
neighbors. Each similarity between an item i and a neighbor j ∈S k (i;u) is com-
puted independently of the content of S k (i;u) and the other similarities: s il for
l ∈ S k (i;u)−{j}. For example, suppose that our items are movies, and the
neighbors set contains three movies that are highly correlated with each other
(e.g., sequels such as “Lord of the Rings 1–3”). An algorithm that ignores the
similarity of the three movies when determining their interpolation weights,
may end up essentially triple counting the information provided by the group.
3. By definition, the interpolation weights sum to one, which may cause overfit-
ting. Suppose that an item has no useful neighbors rated by a particular user. In
that case, it would be best to ignore the neighborhood information, staying with
the more robust baseline predictors. Nevertheless, the standard neighborhood
formula uses a weighted average of ratings for the uninformative neighbors.
4. Neighborhood methods may not work well if variability of ratings differs sub-
stantially among neighbors.
Some of these issues can be fixed to a certain degree, while others are more
difficult to solve within the basic framework. For example, the third item, dealing
with the sum-to-one constraint, can be alleviated by using the following prediction
rule:
ˆ r ui = b ui +
∑ j∈S k (i;u) s ij (r uj −b uj )
λ 9
+∑
j∈S k (i;u) s ij
(5.18)
The constant λ 9 penalizes the neighborhood portion when there is not much neigh-
borhood information, e.g., when ∑ j∈S k (i;u) s ij ≪ λ 9 . Indeed, we have found that
setting an appropriate value of λ 9 leads to accuracy improvements over (5.17).
Nonetheless, the whole framework here is not justified by a formal model. Thus,
5 Advances in Collaborative Filtering 165
we strive for better results with a more fundamental approach, as we describe in the
following.
5.4.3 Jointly derived interpolation weights
In this section we describe a more accurate neighborhood model that overcomes
the difficulties discussed above, while retaining known merits of item-item models.
As above, we use the similarity measure to define neighbors for each prediction.
However, we search for optimum interpolation weights without regard to values of
the similarity measure.
Given a set of neighbors S k (i;u) we need to compute interpolation weights
{ θ u
ij |j ∈ S k (i;u)} that enable the best prediction rule of the form
ˆ r ui = b ui +
∑
j∈S k (i;u)
θ u
ij (r uj −b uj ).
(5.19)
Typical values of k (number of neighbors) lie in the range of 20–50; see [2]. Dur-
ing this subsection we assume that baseline predictors have already been removed.
Hence, we introduce a notation for the residual ratings: z ui
def
= r ui −b ui . For notational
convenience assume that the items in S k (i;u) are indexed by 1,...,k.
We seek a formal computation of the interpolation weights that stems directly
from their usage within prediction rule (5.19). As explained earlier, it is important
to derive all interpolation weights simultaneously to account for interdependencies
among the neighbors. We achieve these goals by defining a suitable optimization
problem.
5.4.3.1 Formal model
To start, we consider a hypothetical dense case, where all users but u rated both i and
all its neighbors in S k (i;u). In that case, we could learn the interpolation weights by
modeling the relationships between item i and its neighbors through a least squares
problem
min
θ u
∑
v̸=u

 z vi −
∑
j∈S k (i;u)
θ u
ij z vj


2
. (5.20)
Notice that the only unknowns here are the θ u
ij ’s. The optimal solution to the least
squares problem (5.20) is found by differentiation as a solution of a linear system
of equations. From a statistics viewpoint, it is equivalent to the result of a linear re-
gression (without intercept) of z vi on the z vj for j ∈S k (i;u). Specifically, the optimal
weights are given by
Aw = b. (5.21)
166 Yehuda Koren and Robert Bell
Here, w ∈ R k is an unknown vector such that w j stands for the sought coefficient
θ u
ij . A is a k×k matrix defined as
A jl = ∑
v̸=u
z vj z vl . (5.22)
Similarly the vector b ∈ R k is given by
b j = ∑
v̸=u
z vj z vi . (5.23)
For a sparse ratings matrix there are likely to be very few users who rated i and all
its neighbors S k (i;u). Accordingly, it would be unwise to base A and b as given in
(5.22)–(5.23) only on users with complete data. Even if there are enough users with
complete data for A to be nonsingular, that estimate would ignore a large proportion
of the information about pairwise relationships among ratings by the same user.
However, we can still estimate A and b, up to the same constant, by averaging over
the given pairwise support, leading to the following reformulation:
¯
A jl =
∑ v∈U(j,l) z vj z vl
|U(j,l)|
(5.24)
¯
b j =
∑ v∈U(i,j) z vj z vi
|U(i, j)|
(5.25)
As a reminder, U(j,l) is the set of users who rated both j and l.
This is still not enough to overcome the sparseness issue. The elements of
¯
A jl
or
¯
b j may differ by orders of magnitude in terms of the number of users included
in the average. As discussed previously, averages based on relatively low support
(small values of |U(j,l)|) can generally be improved by shrinkage towards a com-
mon value. Specifically, we compute a baseline value that is defined by taking the
average of all possible
¯
A jl values. Let us denote this baseline value by avg; its pre-
cise computation is described in the next subsection. Accordingly, we define the
corresponding k×k matrix
ˆ
A and the vector ˆ b ∈ R k :
ˆ
A jl =
|U(j,l)|· ¯ A jl + β ·avg
|U(j,l)|+ β
(5.26)
ˆ
b j =
|U(i, j)|· ¯ b j + β ·avg
|U(i, j)|+ β
(5.27)
The parameter β controls the extent of the shrinkage. A typical value would be
β = 500.
Our best estimate for A and b are
ˆ
A and
ˆ
b, respectively. Therefore, we modify
(5.21) so that the interpolation weights are defined as the solution of the linear sys-
tem
ˆ
Aw = ˆ b. (5.28)
5 Advances in Collaborative Filtering 167
The resulting interpolation weights are used within (5.19) in order to predict r ui .
This method addresses all four concerns raised in Subsection 5.4.2. First, inter-
polation weights are derived directly from the ratings, not based on any similarity
measure. Second, the interpolation weights formula explicitly accounts for relation-
ships among the neighbors. Third, the sum of the weights is not constrained to equal
one. If an item (or user) has only weak neighbors, the estimated weights may all be
very small. Fourth, the method automatically adjusts for variations among items in
their means or variances.
5.4.3.2 Computational issues
Efficientcomputationofanitem-itemneighborhoodmethodrequirespre-computing
certain values associated with each item-item pair for rapid retrieval. First, we need
a quick access to all item-item similarities, by pre-computing all s ij values, as ex-
plained in Subsection 5.4.1.
Second, we pre-compute all possible entries of
ˆ
A and ˆ b. To this end, for each two
items i and j, we compute
¯
A ij =
∑ v∈U(i,j) z vi z vj
|U(i, j)|
.
Then, the aforementioned baseline value avg, which is used in (5.26)-(5.27), is taken
as the average entry of the pre-computed n×n matrix
¯
A. In fact, we recommend
using two different baseline values, one by averaging the non-diagonal entries of
¯
A
and another one by averaging the generally-larger diagonal entries, which have an
inherently higher average because they sum only non-negative values. Finally, we
derive a full n×n matrix
ˆ
A from
¯
A by (5.26), using the appropriate value of avg.
Here, the non-diagonal average is used when deriving the non-diagonal entries of
ˆ
A,
whereas the diagonal average is used when deriving the diagonal entries of
ˆ
A.
Because of symmetry, it is sufficient to store the values of s ij and
ˆ
A ij only for
i> j. Our experience shows that it is enough to allocate one byte for each individual
value, so the overall space required for n items is exactly n(n+1) bytes.
Pre-computing all possible entries of matrix
ˆ
A saves the otherwise lengthy time
needed to construct entries on the fly. After quickly retrieving the relevant entries
of
ˆ
A, we can compute the interpolation weights by solving a k×k system of equa-
tions (5.28) using a standard linear solver. However, a modest increase in prediction
accuracy was achieved when constraining w to be nonnegative through a quadratic
program [2]. Solving the system of equations is an overhead over the basic neigh-
borhood method described in Subsection 5.4.2. For typical values of k (between 20
and 50), the extra time overhead is comparable to the time needed for computing the
k nearest neighbors, which is common to neighborhood-based approaches. Hence,
while the method relies on a much more detailed computation of the interpolation
weights compared to previous methods, it does not significantly increase running
time; see [2].
168 Yehuda Koren and Robert Bell
5.4.4 Summary
Collaborative filtering through neighborhood-based interpolation is probably the
most popular way to create a recommender system. Three major components char-
acterize the neighborhood approach: (1) data normalization, (2) neighbor selection,
and (3) determination of interpolation weights.
Normalization is essential to collaborative filtering in general, and in particular to
the more local neighborhood methods. Otherwise, even more sophisticated methods
are bound to fail, as they mix incompatible ratings pertaining to different unnormal-
ized users or items. We described a suitable approach to data normalization, based
around baseline predictors.
Neighborhood selection is another important component. It is directly related to
the employed similarity measure. Here, we emphasized the importance of shrinking
unreliable similarities, in order to avoid detection of neighbors with a low rating
support.
Finally, the success of neighborhood methods depends on the choice of the in-
terpolation weights, which are used to estimate unknown ratings from neighboring
known ones. Nevertheless, most known methods lack a rigorous way to derive these
weights. We showed how the interpolation weights can be computed as a global
solution to an optimization problem that precisely reflects their role.
5.5 Enriching neighborhood models
Most neighborhood methods are local in their nature – concentrating on only a small
subset of related ratings. This contrasts with matrix factorization, which casts a very
wide net to try to characterize items and users. It appears that accuracy can be im-
provedbyemployingthisglobalviewpoint,whichmotivatesthemethodsofthissec-
tion. We suggest a new neighborhood model drawing on principles of both classical
neighborhood methods and matrix factorization models. Like other neighborhood
models, the building stones here are item-item relations (or, alternatively, user-user
relations), which provide the system some practical advantages discussed earlier. At
the same time, much like matrix factorization, the model is centered around a global
optimization framework, which improves accuracy by considering the many weak
signals existing in the data.
The main method, which is described in Subsection 5.5.1, allows us to enrich the
model with implicit feedback data. In addition, it facilitates two new possibilities.
First is a factorized neighborhood model, as described in Subsection 5.5.2, bringing
great improvements in computational efficiency. Second is a treatment of temporal
dynamics, leading to better prediction accuracy, as described in Subsection 5.5.3.
5 Advances in Collaborative Filtering 169
5.5.1 A global neighborhood model
In this subsection, we introduce a neighborhood model based on global optimiza-
tion. The model offers an improved prediction accuracy, by offering the aforemen-
tioned merits of the model described in Subsection 5.4.3, with additional advantages
that are summarized as follows:
1. No reliance on arbitrary or heuristic item-item similarities. The new model is
cast as the solution to a global optimization problem.
2. Inherent overfitting prevention or “risk control”: the model reverts to robust
baseline predictors, unless a user entered sufficiently many relevant ratings.
3. Themodelcancapturethetotalityofweaksignalsencompassedinallofauser’s
ratings, not needing to concentrate only on the few ratings for most similar
items.
4. The model naturally allows integrating different forms of user input, such as
explicit and implicit feedback.
5. A highly scalable implementation (Section 5.5.2) allows linear time and space
complexity, thus facilitating both item-item and user-user implementations to
scale well to very large datasets.
6. Time drifting aspects of the data can be integrated into the model, thereby im-
proving its accuracy; see Subsection 5.5.3.
5.5.1.1 Building the model
We gradually construct the various components of the model, through an ongoing
refinement of our formulations. Previous models were centered around user-specific
interpolation weights – θ u
ij
in (5.19) or s ij
/∑
j∈S k (i;u) s ij
in (5.17) – relating item
i to the items in a user-specific neighborhood S k (i;u). In order to facilitate global
optimization, we would like to abandon such user-specific weights in favor of global
item-item weights independent of a specific user. The weight from j to i is denoted
by w ij and will be learned from the data through optimization. An initial sketch of
the model describes each rating r ui by the equation
ˆ r ui = b ui +
∑
j∈R(u)
(r uj −b uj )w ij . (5.29)
This rule starts with the crude, yet robust, baseline predictors (b ui ). Then, the
estimate is adjusted by summing over all ratings by u.
Let us consider the interpretation of the weights. Usually the weights in a neigh-
borhood model represent interpolation coefficients relating unknown ratings to ex-
isting ones. Here, we adopt a different viewpoint, that enables a more flexible usage
of the weights. We no longer treat weights as interpolation coefficients. Instead, we
take weights as part of adjustments, or offsets, added to the baseline predictors. This
way, the weight w ij is the extent by which we increase our baseline prediction of r ui
based on the observed value of r uj . For two related items i and j, we expect w ij to
170 Yehuda Koren and Robert Bell
be high. Thus, whenever a user u rated j higher than expected (r uj −b uj is high), we
would like to increase our estimate for u’s rating of i by adding (r uj −b uj )w ij to the
baseline prediction. Likewise, our estimate will not deviate much from the baseline
by an item j that u rated just as expected (r uj −b uj is around zero), or by an item j
that is not known to be predictive on i (w ij is close to zero).
This viewpoint suggests several enhancements to (5.29). First, we can use the
form of binary user input, which was found beneficial for factorization models.
Namely, analyzing which items were rated regardless of rating value. To this end,
we add another set of weights, and rewrite (5.29) as
ˆ r ui = b ui +
∑
j∈R(u)
[(r uj −b uj )w ij +c ij ]. (5.30)
Similarly, one could employ here another set of implicit feedback, N(u)—e.g.,
the set of items rented or purchased by the user—leading to the rule
ˆ r ui = b ui +
∑
j∈R(u)
(r uj −b uj )w ij +
∑
j∈N(u)
c ij . (5.31)
Much like the w ij ’s, the c ij ’s are offsets added to the baseline predictor. For two
items i and j, an implicit preference by u for j leads us to adjust our estimate of r ui
by c ij , which is expected to be high if j is predictive on i.
Employing global weights, rather than user-specific interpolation coefficients,
emphasizes the influence of missing ratings. In other words, a user’s opinion is
formed not only by what he rated, but also by what he did not rate. For example,
suppose that a movie ratings dataset shows that users that rate “Shrek 3” high also
gave high ratings to “Shrek 1–2”. This will establish high weights from “Shrek 1–2”
to “Shrek 3”. Now, if a user did not rate “Shrek 1–2” at all, his predicted rating for
“Shrek 3” will be penalized, as some necessary weights cannot be added to the sum.
For prior models (5.17) and (5.19) that interpolated r ui −b ui from {r uj −b uj |j ∈
S k (i;u)}, it was necessary to maintain compatibility between the b ui values and
the b uj values. However, here we do not use interpolation, so we can decouple the
definitions of b ui and b uj . Accordingly, a more general prediction rule would be:
ˆ r ui = ˜ b ui
+∑
j∈R(u) (r uj −b uj )w ij +c ij . The constant
˜
b ui can represent predictions of
r ui by other methods such as a latent factor model. Here, we suggest the following
rule that was found to work well:
ˆ r ui = µ +b u +b i +
∑
j∈R(u)
[(r uj −b uj )w ij +c ij ] (5.32)
Importantly, the b uj ’s remain constants, which are derived as explained in Section
5.2.1. However, the b u ’s and b i ’s become parameters that are optimized much like
the w ij ’s and c ij ’s.
We have found that it is beneficial to normalize sums in the model leading to the
form
5 Advances in Collaborative Filtering 171
ˆ r ui = µ +b u +b i +|R(u)| − α
∑
j∈R(u)
[(r uj −b uj )w ij +c ij ]. (5.33)
The constant α controls the extent of normalization. A non-normalized rule ( α =
0), encourages greater deviations from baseline predictions for users that provided
manyratings(high|R(u)|).Ontheotherhand,afullynormalizedrule,eliminatesthe
effect of number of ratings on deviations from baseline predictions. In many cases it
would be a good practice for recommender systems to have greater deviation from
baselines for users that rate a lot. This way, we take more risk with well modeled
users that provided much input. For such users we are willing to predict quirkier
and less common recommendations. At the same time, we are less certain about the
modeling of users that provided only a little input, in which case we would like to
stay with safe estimates close to the baseline values. Our experience with the Netflix
dataset shows that best results are achieved with α = 0.5, as in the prediction rule
ˆ r ui = µ +b u +b i +|R(u)| −
1
2
∑
j∈R(u)
[(r uj −b uj )w ij +c ij ]. (5.34)
As an optional refinement, complexity of the model can be reduced by pruning
parameters corresponding to unlikely item-item relations. Let us denote by S k (i)
the set of k items most similar to i, as determined by e.g., a similarity measure s ij
or a natural hierarchy associated with the item set. Additionally, we use R k (i;u)
def
=
R(u)∩S k (i). 2 Now, when predicting r ui according to (5.34), it is expected that the
most influential weights will be associated with items similar to i. Hence, we replace
(5.34) with
ˆ r ui = µ +b u +b i +|R k (i;u)| −
1
2
∑
j∈R k (i;u)
[(r uj −b uj )w ij +c ij ]. (5.35)
When k = ∞, rule (5.35) coincides with (5.34). However, for other values of k it
offers the potential to significantly reduce the number of variables involved.
5.5.1.2 Parameter Estimation
Prediction rule 5.35 allows fast online prediction. More computational work is
needed at a pre-processing stage where parameters are estimated. A major design
goal of the new neighborhood model was facilitating an efficient global optimiza-
tion procedure, which prior neighborhood models lacked. Thus, model parameters
are learned by solving the regularized least squares problem associated with (5.35):
2
Notational clarification: With other neighborhood models it was beneficial to use S k (i;u), which
denotes the k items most similar to i among those rated by u. Hence, if u rated at least k items, we
will always have |S k (i;u)| = k, regardless of how similar those items are to i. However, |R k (i;u)|
is typically smaller than k, as some of those items most similar to i were not rated by u.
172 Yehuda Koren and Robert Bell
min
b ∗ ,w ∗ ,c ∗
∑
(u,i)∈ K
(
r ui − µ −b u −b i −|R k (i;u)| −
1
2
∑
j∈R k (i;u)
((r uj −b uj )w ij +c ij )
) 2
+ λ 10
(
b 2
u +b 2 i + ∑
j∈R k (i;u)
w 2
ij +c 2 ij
)
(5.36)
An optimal solution of this convex problem can be obtained by least square
solvers, which are part of standard linear algebra packages. However, we have found
that the following simple stochastic gradient descent solver works much faster. Let
us denote the prediction error, r ui − ˆ r ui , by e ui . We loop through all known ratings in
K. For a given training case r ui , we modify the parameters by moving in the opposite
direction of the gradient, yielding:
• b u ← b u + γ ·(e ui − λ 10 ·b u )
• b i ← b i + γ ·(e ui − λ 10 ·b i )
• ∀j ∈ R k (i;u) :
w ij ← w ij + γ ·
(
|R k (i;u)| −
1
2 ·e ui ·(r uj −b uj )− λ 10 ·w ij
)
c ij ← c ij + γ ·
(
|R k (i;u)| −
1
2 ·e ui − λ 10 ·c ij
)
Themeta-parameters γ (stepsize)and λ 10 aredeterminedbycross-validation.We
used γ = 0.005 and λ 10 = 0.002 for the Netflix data. Another important parameter
is k, which controls the neighborhood size. Our experience shows that increasing
k always benefits the accuracy of the results on the test set. Hence, the choice of
k should reflect a tradeoff between prediction accuracy and computational cost. In
Subsection 5.5.2 we will describe a factored version of the model that eliminates
this tradeoff by allowing us to work with the most accurate k = ∞ while lowering
running time.
A typical number of iterations throughout the training data is 15–20. As for time
complexity per iteration, let us analyze the most accurate case where k = ∞, which
is equivalent to using prediction rule (5.34). For each user u and item i ∈ R(u) we
need to modify {w ij ,c ij |j ∈ R(u)}. Thus the overall time complexity of the training
phase is O(∑ u |R(u)| 2 ).
5.5.1.3 Comparison of accuracy
Experimental results on the Netflix data with the globally optimized neighborhood
model, henceforth dubbed GlobalNgbr, are presented in Figure 5.1. We studied the
model under different values of parameter k. The solid black curve with square
symbols shows that accuracy monotonically improves with rising k values, as root
mean squared error (RMSE) falls from 0.9139 for k = 250 to 0.9002 for k = ∞.
(Notice that since the Netflix data contains 17,770 movies, k = ∞ is equivalent to
k=17,769, where all item-item relations are explored.) We repeated the experiments
without using the implicit feedback, that is, dropping the c ij parameters from our
5 Advances in Collaborative Filtering 173
model. The results depicted by the solid black curve with X’s show a significant
decline in estimation accuracy, which widens as k grows. This demonstrates the
value of incorporating implicit feedback into the model.
For comparison we provide the results of the two previously described neighbor-
hood models. First is a similarity-based neighborhood model (in Subsection 5.4.2),
which is the most popular CF method in the literature. We denote this model as
CorNgbr. Second is the more accurate model described in Subsection 5.4.3, which
will be denoted as JointNgbr. For both these two models, we tried to pick optimal
parameters and neighborhood sizes, which were 20 for CorNgbr, and 50 for Joint-
Ngbr. The results are depicted by the dotted and dashed lines, respectively. It is clear
that the popular CorNgbr method is noticeably less accurate than the other neigh-
borhood models. On the opposite side, GlobalNgbr delivers more accurate results
even when compared with JointNgbr, as long as the value of k is at least 500. Notice
that the k value (the x-axis) is irrelevant to the previous models, as their different
notion of neighborhood makes neighborhood sizes incompatible. Yet, we observed
that while the performance of GlobalNgbr keeps improving as more neighbors are
added, this was not true with the two other models. For CorNgbr and JointNgbr,
performance peaks with a relatively small number of neighbors and declines ther-
after. This may be explained by the fact that in GlobalNgbr, parameters are directly
learned from the data through a formal optimization procedure that facilitates using
many more parameters effectively.
Finally, let us consider running time. Previous neighborhood models require very
light pre-processing, though, JointNgbr [2] requires solving a small system of equa-
tions for each provided prediction. The new model does involve pre-processing
where parameters are estimated. However, online prediction is immediate by fol-
lowing rule (5.35). Pre-processing time grows with the value of k. Figure 5.2 shows
typical running times per iteration on the Netflix data, as measured on a single pro-
cessor 3.4GHz Pentium 4 PC.
5.5.2 A factorized neighborhood model
In the previous subsection we presented a more accurate neighborhood model,
whichisbasedonpredictionrule(5.34)withtrainingtimecomplexityO(∑ u |R(u)| 2 )
and space complexity O(m+n 2 ). (Recall that m is the number of users, and n is the
number of items.) We could improve time and space complexity by sparsifying the
model through pruning unlikely item-item relations. Sparsification was controlled
by the parameter k 6 n, which reduced running time and allowed space complex-
ity of O(m+nk). However, as k gets lower, the accuracy of the model declines as
well. In addition, sparsification required relying on an external, less natural, simi-
larity measure, which we would have liked to avoid. Thus, we will now show how
to retain the accuracy of the full dense prediction rule (5.34), while significantly
lowering time and space complexity.
174 Yehuda Koren and Robert Bell
0.9
0.905
0.91
0.915
0.92
0.925
0.93
0.935
0.94
0.945
250 500 1000 2000 4000 8000 ∞
RMSE
k
GlobalNgbr
GlobalNgbr w/o implicit
JointNgbr
CorNgbr
Fig. 5.1: Comparison of neighborhood-based models. Accuracy is measured by
RMSE on the Netflix test set, so lower values indicate better performance. We mea-
sure the accuracy of the globally optimized model (GlobalNgbr) with and without
implicit feedback. RMSE is shown as a function of varying values of k, which dic-
tates the neighborhood size. The accuracy of two other models is shown as two
horizontal lines; for each we picked an optimal neighborhood size.
5.5.2.1 Factoring item-item relationships
We factor item-item relationships by associating each item i with three vectors:
q i ,x i ,y i ∈ R f . This way, we confine w ij to be q T
i
x i . Similarly, we impose the struc-
ture c ij = q T
i
y j . Essentially, these vectors strive to map items into an f-dimensional
latent factor space where they are measured against various aspects that are revealed
automatically by learning from the data. By substituting this into (5.34) we get the
following prediction rule:
ˆ r ui = µ +b u +b i +|R(u)| −
1
2
∑
j∈R(u)
[(r uj −b uj )q T
i x j +q T i y j ]
(5.37)
Computational gains become more obvious by using the equivalent rule
ˆ r ui = µ +b u +b i +q T
i
(
|R(u)| −
1
2
∑
j∈R(u)
(r uj −b uj )x j +y j
)
. (5.38)
5 Advances in Collaborative Filtering 175
10
20
30
40
50
60
250 500 1000 2000 4000 8000 ∞
time per iteration (minutes)
k
Fig. 5.2: Running time per iteration of the globally optimized neighborhood model,
as a function of the parameter k.
Notice that the bulk of the rule (|R(u)| −
1
2 ∑ j∈R(u) (r uj −b uj )x j +y j ) depends only
on u while being independent of i. This leads to an efficient way to learn the model
parameters. As usual, we minimize the regularized squared error function associated
with (5.38)
min
q ∗ ,x ∗ ,y ∗ ,b ∗
∑
(u,i)∈ K
(
r ui − µ −b u −b i −q T
i
(
|R(u)| −
1
2
∑
j∈R(u)
(r uj −b uj )x j +y j
) ) 2
+ λ 11
(
b 2
u +b 2 i +∥q i ∥ 2 + ∑
j∈R(u)
∥x j ∥ 2 +∥y j ∥ 2
)
. (5.39)
Optimization is done by a stochastic gradient descent scheme, which is described
in the following pseudo code:
176 Yehuda Koren and Robert Bell
LearnFactorizedNeighborhoodModel(Known ratings: r ui , rank: f)
% For each item i compute q i ,x i ,y i ∈ R f
% which form a neighborhood model
Const #Iterations = 20, γ = 0.002, λ = 0.04
% Gradient descent sweeps:
for count = 1,...,#Iterations do
for u = 1,...,m do
% Compute the component independent of i:
p u ← |R(u)| −
1
2 ∑ j∈R(u) (r uj −b uj )x j +y j
sum ← 0
for all i ∈ R(u) do
ˆ r ui ← µ +b u +b i +q T
i
p u
e ui ← r ui − ˆ r ui
% Accumulate information for gradient steps on x i ,y i :
sum ← sum+e ui ·q i
% Perform gradient step on q i ,b u ,b i :
q i ← q i + γ ·(e ui · p u − λ ·q i )
b u ← b u + γ ·(e ui − λ ·b u )
b i ← b i + γ ·(e ui − λ ·b i )
for all i ∈ R(u) do
% Perform gradient step on x i :
x i ← x i + γ ·(|R(u)| −
1
2 ·(r ui −b ui )·sum− λ ·x i )
% Perform gradient step on y i :
y i ← y i + γ ·(|R(u)| −
1
2 ·sum− λ ·y i )
return {q i ,x i ,y i |i = 1,...,n}
The time complexity of this model is linear with the input size, O(f ·∑ u (|R(u)|)),
which is significantly better than the non-factorized model that required time
O(∑
u |R(u)| 2 ). We measured the performance of the model on the Netflix data; see
Table 5.3. Accuracy is improved as we use more factors (increasing f). However,
going beyond 200 factors could barely improve performance, while slowing running
time. Interestingly, we have found that with f >200 accuracy negligibly exceeds the
best non-factorized model (with k = ∞). In addition, the improved time complexity
translates into a big difference in wall-clock measured running time. For example,
the time-per-iteration for the non-factorized model (with k = ∞) was close to 58
minutes. On the other hand, a factorized model with f = 200 could complete an
iteration in 14 minutes without degrading accuracy at all.
The most important benefit of the factorized model is the reduced space com-
plexity, which is O(m+nf) – linear in the input size. Previous neighborhood models
required storing all pairwise relations between items, leading to a quadratic space
complexity of O(m+n 2 ). For the Netflix dataset which contains 17,770 movies,
such quadratic space can still fit within core memory. Some commercial recom-
menders process a much higher number of items. For example, an online movie
5 Advances in Collaborative Filtering 177
rental service like Netflix is currently offering over 100,000 titles. Music down-
load shops offer even more titles. Such more comprehensive systems with data on
100,000s items eventually need to resort to external storage in order to fit the en-
tire set of pairwise relations. However, as the number of items is growing towards
millions, as in the Amazon item-item recommender system, which accesses stored
similarity information for several million catalog items [20], designers must keep
a sparse version of the pairwise relations. To this end, only values relating an item
to its top-k most similar neighbors are stored thereby reducing space complexity to
O(m+nk). However, a sparsification technique will inevitably degrade accuracy by
missing important relations, as demonstrated in the previous section. In addition,
identification of the top k most similar items in such a high dimensional space is a
non-trivial task that can require considerable computational efforts. All these issues
do not exist in our factorized neighborhood model, which offers a linear time and
space complexity without trading off accuracy.
#factors 50 100 200 500
RMSE 0.9037 0.9013 0.9000 0.8998
time/iteration 4.5 min 8 min 14 min 34 min
Table 5.3: Performance of the factorized item-item neighborhood model. The mod-
els with > 200 factors slightly outperform the non-factorized model, while provid-
ing much shorter running time.
The factorized neighborhood model resembles some latent factor models. The
important distinction is that here we factorize the item-item relationships, rather
than the ratings themselves. The results reported in Table 5.3 are comparable to
those of the widely used SVD model, but not as good as those of SVD++; see
Section 5.3. Nonetheless, the factorized neighborhood model retains the practical
advantages of traditional neighborhood models discussed earlier—the abilities to
explain recommendations and to immediately reflect new ratings.
As a side remark, we would like to mention that the decision to use three separate
sets of factors was intended to give us more flexibility. Indeed, on the Netflix data
this allowed achieving most accurate results. However, another reasonable choice
could be using a smaller set of vectors, e.g., by requiring: q i = x i (implying sym-
metric weights: w ij = w ji ).
5.5.2.2 A user-user model
A user-user neighborhood model predicts ratings by considering how like-minded
users rated the same items. Such models can be implemented by switching the roles
ofusersanditemsthroughoutourderivationoftheitem-itemmodel.Here,wewould
like to concentrate on a user-user model, which is dual to the item-item model of
178 Yehuda Koren and Robert Bell
(5.34). The major difference is replacing the w ij weights relating item pairs, with
weights relating user pairs:
ˆ r ui = µ +b u +b i +|R(i)| −
1
2
∑
v∈R(i)
(r vi −b vi )w uv (5.40)
The set R(i) contains all the users who rated item i. Notice that here we decided
to not account for implicit feedback. This is because adding such feedback was not
very beneficial for the user-user model when working with the Netflix data.
User-user models can become useful in various situations. For example, some
recommenders may deal with items that are rapidly replaced, thus making item-item
relations veryvolatile. On the other hand, a stable user base enables establishment of
long term relationships between users. An example of such a case is a recommender
system for web articles or news items, which are rapidly changing by their nature;
see, e.g., [8]. In such cases, systems centered around user-user relations are more
appealing.
In addition, user-user approaches identify different kinds of relations that item-
item approaches may fail to recognize, and thus can be useful on certain occasions.
For example, suppose that we want to predict r ui , but none of the items rated by
user u is really relevant to i. In this case, an item-item approach will face obvious
difficulties. However, when employing a user-user perspective, we may find a set
of users similar to u, who rated i. The ratings of i by these users would allow us to
improve prediction of r ui .
The major disadvantage of user-user models is computational. Since typically
there are many more users than items, pre-computing and storing all user-user re-
lations, or even a reasonably sparsified version thereof, is overly expensive or com-
pletely impractical. In addition to the high O(m 2 ) space complexity, the time com-
plexity for optimizing model (5.40) is also much higher than its item-item coun-
terpart, being O(∑ i |R(i)| 2 ) (notice that |R(i)| is expected to be much higher than
|R(u)|). These issues have rendered user-user models as a less practical choice.
A factorized model. All those computational differences disappear by factoriz-
ing the user-user model along the same lines as in the item-item model. Now, we
associate each user u with two vectors p u ,z u ∈ R f . We assume the user-user rela-
tions to be structured as: w uv = p T
u z v . Let us substitute this into (5.40) to get
ˆ r ui = µ +b u +b i +|R(i)| −
1
2
∑
v∈R(i)
(r vi −b vi )p T
u z v .
(5.41)
Onceagain,anefficientcomputationis achievedbyincluding theterms thatdepends
on i but are independent of u in a separate sum, so the prediction rule is presented
in the equivalent form
ˆ r ui = µ +b u +b i + p T
u |R(i)| −
1
2
∑
v∈R(i)
(r vi −b vi )z v . (5.42)
5 Advances in Collaborative Filtering 179
In a parallel fashion to the item-item model, all parameters are learned in linear time
O(f ·∑ i |R(i)|). The space complexity is also linear with the input size being O(n+
mf). This significantly lowers the complexity of the user-user model compared to
previously known results. In fact, running time measured on the Netflix data shows
that now the user-user model is even faster than the item-item model; see Table 5.4.
We should remark that unlike the item-item model, our implementation of the user-
user model did not account for implicit feedback, which probably led to its shorter
running time. Accuracy of the user-user model is significantly better than that of
the widely-used correlation-based item-item model that achieves RMSE=0.9406 as
reported in Figure 5.1. Furthermore, accuracy is slightly better than the variant of
the item-item model, which also did not account for implicit feedback (yellow curve
in Figure 5.1). This is quite surprising given the common wisdom that item-item
methods are more accurate than user-user ones. It appears that a well implemented
user-user model can match speed and accuracy of an item-item model. However, our
item-item model could significantly benefit by accounting for implicit feedback.
#factors 50 100 200 500
RMSE 0.9119 0.9110 0.9101 0.9093
time/iteration 3 min 5 min 8.5 min 18 min
Table 5.4: Performance of the factorized user-user neighborhood model.
Fusing item-item and user-user models. Since item-item and user-user models
address different aspects of the data, overall accuracy is expected to improve by
combining predictions of both models. Such an approach was previously suggested
and was shown to improve accuracy; see, e.g. [4, 32]. However, past efforts were
based on blending the item-item and user-user predictions during a post-processing
stage, after each individual model was trained independently of the other model.
A more principled approach optimizes the two models simultaneously, letting them
know of each other while parameters are being learned. Thus, throughout the entire
training phase each model is aware of the capabilities of the other model and strives
to complement it. Our approach, which states the neighborhood models as formal
optimization problems, allows doing that naturally. We devise a model that sums the
item-item model (5.37) and the user-user model (5.41), leading to
ˆ r ui = µ +b u +b i +|R(u)| −
1
2
∑
j∈R(u)
[(r uj −b uj )q T
i x j +q T i y j ]
+|R(i)| −
1
2
∑
v∈R(i)
(r vi −b vi )p T
u z v .
(5.43)
Model parameters are learned by stochastic gradient descent optimization of the
associated squared error function. Our experiments with the Netflix data show that
prediction accuracy is indeed better than that of each individual model. For example,
180 Yehuda Koren and Robert Bell
with 100 factors the obtained RMSE is 0.8966, while with 200 factors the obtained
RMSE is 0.8953.
Here we would like to comment that our approach allows integrating the neigh-
borhood models also with completely different models in a similar way. For ex-
ample, in [17] we showed an integrated model that combines the item-item model
with a latent factor model (SVD++), thereby achieving improved prediction accu-
racy with RMSE below 0.887. Therefore, other possibilities with potentially better
accuracy should be explored before considering the integration of item-item and
user-user models.
5.5.3 Temporal dynamics at neighborhood models
Oneoftheadvantagesoftheitem-itemmodelbasedonglobaloptimization(Subsec-
tion 5.5.1), is that it enables us to capture temporal dynamics in a principled manner.
As we commented earlier, user preferences are drifting over time, and hence it is im-
portant to introduce temporal aspects into CF models.
When adapting rule (5.34) to address temporal dynamics, two components
should be considered separately. First component, µ +b i +b u , corresponds to the
baseline predictor portion. Typically, this component explains most variability in
the observed signal. Second component, |R(u)| −
1
2 ∑ j∈R(u) (r uj −b uj )w ij +c ij , cap-
tures the more informative signal, which deals with user-item interaction. As for
the baseline part, nothing changes from the factor model, and we replace it with
µ +b i (t ui )+b u (t ui ), according to (5.6) and (5.9). However, capturing temporal dy-
namics within the interaction part requires a different strategy.
Item-item weights (w ij and c ij ) reflect inherent item characteristics and are not
expected to drift over time. The learning process should capture unbiased long term
values, without being too affected from drifting aspects. Indeed, the time changing
nature of the data can mask much of the longer term item-item relationships if not
treated adequately. For instance, a user rating both items i and j high within a short
time period, is a good indicator for relating them, thereby pushing higher the value
of w ij . On the other hand, if those two ratings are given five years apart, while
the user’s taste (if not her identity) could considerably change, this provides less
evidenceofanyrelationbetweentheitems.Ontopofthis,wewouldarguethatthose
considerations are pretty much user-dependent; some users are more consistent than
others and allow relating their longer term actions.
Our goal here is to distill accurate values for the item-item weights, despite the
interfering temporal effects. First we need to parameterize the decaying relations
between two items rated by user u. We adopt exponential decay formed by the
function e − β u · ∆ t , where β u > 0 controls the user specific decay rate and should be
learned from the data. We also experimented with other decay forms, like the more
computationally-friendly (1+ β u ∆ t) −1 , which resulted in about the same accuracy,
with an improved running time.
This leads to the prediction rule
5 Advances in Collaborative Filtering 181
ˆ r ui = µ +b i (t ui )+b u (t ui )+|R(u)| −
1
2
∑
j∈R(u)
e − β u ·|t ui −t uj | ((r uj −b uj )w ij +c ij ). (5.44)
The involved parameters, b i (t ui ) = b i + b i,Bin(t ui ) , b u (t ui ) = b u + α u · dev u (t ui ) +
b u,t ui , β u , w ij and c ij , are learned by minimizing the associated regularized squared
error
∑
(u,i)∈ K
(
r ui − µ −b i −b i,Bin(t ui ) −b u − α u dev u (t ui )−b u,t ui −
|R(u)| −
1
2
∑
j∈R(u)
e − β u ·|t ui −t uj | ((r uj −b uj )w ij +c ij )
) 2
+
λ 12
(
b 2
i +b 2 i,Bin(t ui ) +b 2 u + α 2 u +b 2 u,t +w 2 ij +c 2 ij
)
. (5.45)
Minimization is performed by stochastic gradient descent. We run the process for
25 iterations, with λ 12 = 0.002, and step size (learning rate) of 0.005. An exception
is the update of the exponent β u , where we are using a much smaller step size of
10 −7 . Training time complexity is the same as the original algorithm, which is:
O(∑
u |R(u)| 2 ). One can tradeoff complexity with accuracy by sparsifying the set of
item-item relations as explained in Subsection 5.5.1.
As in the factor case, properly considering temporal dynamics improves the ac-
curacy of the neighborhood model within the movie ratings dataset. The RMSE
decreases from 0.9002 [17] to 0.8885. To our best knowledge, this is significantly
better than previously known results by neighborhood methods. To put this in
some perspective, this result is even better than those reported by using hybrid
approaches such as applying a neighborhood approach on residuals of other algo-
rithms [2, 23, 31]. A lesson is that addressing temporal dynamics in the data can
have a more significant impact on accuracy than designing more complex learning
algorithms.
We would like to highlight an interesting point. Let u be a user whose preferences
are quickly drifting ( β u is large). Hence, old ratings by u should not be very influen-
tial on his status at the current time t. One could be tempted to decay the weight of
u’s older ratings, leading to “instance weighting” through a cost function like
∑
(u,i)∈ K
e − β u ·|t−t ui |
(
r ui − µ −b i −b i,Bin(t ui ) −b u − α u dev u (t ui )−
b u,t ui −|R(u)| −
1
2
∑
j∈R(u)
((r uj −b uj )w ij +c ij )
) 2
+ λ 12 (···).
Such a function is focused at the current state of the user (at time t), while de-
emphasizing past actions. We would argue against this choice, and opt for equally
weighting the prediction error at all past ratings as in (5.45), thereby modeling all
past user behavior. Therefore, equal-weighting allows us to exploit the signal at each
of the past ratings, a signal that is extracted as item-item weights. Learning those
182 Yehuda Koren and Robert Bell
weights would equally benefit from all ratings by a user. In other words, we can
deduce that two items are related if users rated them similarly within a short time
frame, even if this happened long ago.
5.5.4 Summary
This section follows a less traditional neighborhood based model, which unlike pre-
vious neighborhood methods is based on formally optimizing a global cost func-
tion. The resulting model is no longer localized, considering relationships between
a small set of strong neighbors, but rather considers all possible pairwise relations.
This leads to improved prediction accuracy, while maintaining some merits of the
neighborhood approach such as explainability of predictions and ability to handle
new ratings (or new users) without re-training the model.
The formal optimization framework offers several new possibilities. First, is a
factorized version of the neighborhood model, which improves its computational
complexity while retaining prediction accuracy. In particular, it is free from the
quadratic storage requirements that limited past neighborhood models.
Second addition is the incorporation of temporal dynamics into the model. In or-
der to reveal accurate relations among items, a proposed model learns how influence
between two items rated by a user decays over time. Much like in the matrix factor-
ization case, accounting for temporal effects results in a significant improvement in
predictive accuracy.
5.6 Between neighborhood and factorization
This chapter was structured around two different approaches to CF: factorization
and neighborhood. Each approach evolved from different basic principles, which
led to distinct prediction rules. We also argued that factorization can lead to some-
what more accurate results, while neighborhood models may have some practical
advantages. In this section we will show that despite those differences, the two ap-
proaches share much in common. After all, they are both linear models.
Let us consider the SVD model of Subsection 5.3.1, based on
ˆ r ui = q T
i
p u . (5.46)
For simplicity, we ignore here the baseline predictors, but one can easily reintroduce
them or just assume that they were subtracted from all ratings at an earlier stage.
We arrange all item-factors within the n× f matrix Q = [q 1 q 2 ...q n ] T . Similarly,
we arrange all user-factors within the m× f matrix P = [p 1 p 2 ...p m ] T . We use the
n u × f matrix Q[u] to denote the restriction of Q to the items rated by u, where
n u = |R(u)|. Let the vector r u ∈ R n u contain the ratings given by u ordered as in
5 Advances in Collaborative Filtering 183
Q[u]. Now, by activating (5.46) on all ratings given by u, we can reformulate it in a
matrix form
ˆ r u = Q[u]p u (5.47)
For Q[u] fixed, ∥r u −Q[u]p u ∥ 2 is minimized by
p u = (Q[u] T Q[u]) −1 Q[u] T r u
In practice, we will regularize with λ > 0 to get
p u = (Q[u] T Q[u]+ λ I) −1 Q[u] T r u .
By substituting p u in (5.47) we get
ˆ r u = Q[u](Q[u] T Q[u]+ λ I) −1 Q[u] T r u . (5.48)
This expression can be simplified by introducing some new notation. Let us de-
note the f × f matrix (Q[u] T Q[u]+ λ I) −1 as W u , which should be considered as a
weighting matrix associated with user u. Accordingly, the weighted similarity be-
tween items i and j from u’s viewpoint is denoted by s u
ij = q T i W
u q j . Using this new
notation and (5.48) the predicted preference of u for item i by SVD is rewritten as
ˆ r ui =
∑
j∈R(u)
s u
ij r uj .
(5.49)
We reduced the SVD model into a linear model that predicts preferences as a linear
function of past actions, weighted by item-item similarity. Each past action receives
a separate term in forming the prediction ˆ r ui . This is equivalent to an item-item
neighborhood model. Quite surprisingly, we transformed the matrix factorization
model into an item-item model, which is characterized by:
• Interpolation is made from all past user ratings, not only from those associated
with items most similar to the current one.
• The weight relating items i and j is factorized as a product of two vectors, one
related to i and the other to j.
• Item-item weights are subject to a user-specific normalization, through the ma-
trixW u .
Those properties support our findings on how to best construct a neighborhood
model. First, we showed in Subsection 5.5.1 that best results for neighborhood
models are achieved when the neighborhood size (controlled by constant k) is max-
imal, such that all past user ratings are considered. Second, in Subsection 5.5.2 we
touted the practice of factoring item-item weights. As for the user-specific normal-
ization, we used a simpler normalizer: n −0.5
u
. It is likely that SVD suggests a more
fundamental normalization by the matrix W u , which would work better. However,
computingW u would be expensive in practice. Another difference between our sug-
gested item-item model and the one implied by SVD is that we chose to work with
asymmetric weights (w ij ̸= w ji ), whereas in the SVD-induced rule: s u
ij = s u ji .
184 Yehuda Koren and Robert Bell
In the derivation above we showed how SVD induces an equivalent item-item
technique. In a fully analogous way, it can induce an equivalent user-user technique,
by expressing q i as a function of the ratings and user factors. This brings us to
three equivalent models: SVD, item-item and user-user. Beyond linking SVD with
neighborhoodmodels,thisalsoshowsthatuser-useranditem-itemapproaches,once
well designed, are equivalent.
This last relation (between user-user and item-item approaches) can also be ap-
proached intuitively. Neighborhood models try to relate users to new items by fol-
lowing chains of user-item adjacencies. Such adjacencies represent preference- or
rating-relations between the respective users and items. Both user-user and item-
item models act by following exactly the same chains. They only differ in which
“shortcuts” are exploited to speed up calculations. For example, recommending
itemB to user1 would follow the chain user1–itemA–user2–itemB (user1 rated
itemA, which was also rated by user2, who rated itemB). A user-user model fol-
lows such a chain with pre-computed user-user similarities. This way, it creates a
“shortcut” that bypasses the sub-chain user1–itemB–user2, replacing it with a sim-
ilarity value between user1 and user2. Analogously, an item-item approach follows
exactly the same chain, but creates an alternative “shortcut”, replacing the sub-chain
itemA–user2–itemB with an itemA–itemB similarity value.
Another lesson here is that the distinction that deems neighborhood models as
“memory based”, while taking matrix factorization and the likes as “model based”
isnotalwaysappropriate,atleastnotwhenusingaccurateneighborhoodmodelsthat
are model-based as much as SVD. In fact, the other direction is also true. The bet-
ter matrix factorization models, such as SVD++, are also following memory-based
habits, as they sum over all memory stored ratings when doing the online predic-
tion; see rule (5.3). Hence, the traditional separation between “memory based” and
“model based” techniques is not appropriate for categorizing the techniques sur-
veyed in this chapter.
So far, we concentrated on relations between neighborhood models and matrix
factorization models. However, in practice it may be beneficial to break these rela-
tions, and to augment factorization models with sufficiently different neighborhood
models that are able to complement them. Such a combination can lead to improved
prediction accuracy [3, 17]. A key to achieve this is by using the more localized
neighborhood models (those of Section 5.4, rather than those of Section 5.5), where
the number of considered neighbors is limited. The limited number of neighbors
might not be the best way to construct a standalone neighborhood model, but it
makes the neighborhood model different enough from the factorization model in
order to add a local perspective that the rather global factorization model misses.
References
1. Ali, K., and van Stam, W., “TiVo: Making Show Recommendations Using a Distributed Col-
laborative Filtering Architecture”, Proc. 10th ACM SIGKDD Int. Conference on Knowledge
5 Advances in Collaborative Filtering 185
Discovery and Data Mining, pp. 394–401, 2004.
2. Bell, R., and Koren, Y., “Scalable Collaborative Filtering with Jointly Derived Neighborhood
Interpolation Weights”, IEEE International Conference on Data Mining (ICDM’07), pp. 43–
52, 2007.
3. Bell, R., and Koren, Y., “Lessons from the Netflix Prize Challenge”, SIGKDD Explorations 9
(2007), 75–79.
4. Bell, R.M., Koren, Y., and Volinsky, C., “Modeling Relationships at Multiple Scales to Im-
prove Accuracy of Large Recommender Systems”, Proc. 13th ACM SIGKDD International
Conference on Knowledge Discovery and Data Mining, 2007.
5. Bennet, J., and Lanning, S., “The Netflix Prize”, KDD Cup and Workshop, 2007. www.
netflixprize.com.
6. Canny, J., “Collaborative Filtering with Privacy via Factor Analysis”, Proc. 25th ACM SIGIR
Conf. on Research and Development in Information Retrieval (SIGIR’02), pp. 238–245, 2002.
7. Blei, D., Ng, A., and Jordan, M., “Latent Dirichlet Allocation”, Journal of Machine Learning
Research 3 (2003), 993–1022.
8. Das, A., Datar, M., Garg, A., and Rajaram, S., “Google News Personalization: Scalable Online
Collaborative Filtering”, WWW’07, pp. 271-280, 2007.
9. Deerwester, S., Dumais, S., Furnas, G.W., Landauer, T.K. and Harshman, R., “Indexing by
Latent Semantic Analysis”, Journal of the Society for Information Science 41 (1990), 391–
407.
10. Funk,S.,“NetflixUpdate:TryThisAtHome”,http://sifter.org/˜simon/journal/20061211.html,
2006.
11. Gelman, A., Carlin, J.B., Stern, H.S., and Rubin, D.B., Bayesian Data Analysis, Chapman and
Hall, 1995.
12. Goldberg, D., Nichols, D., Oki, B.M., and Terry, D., “Using Collaborative Filtering to Weave
an Information Tapestry”, Communications of the ACM 35 (1992), 61–70.
13. Herlocker, J.L., Konstan, J.A., and Riedl, J., “Explaining Collaborative Filtering Recommen-
dations”, Proc. ACM Conference on Computer Supported Cooperative Work, pp. 241–250,
2000.
14. Herlocker, J.L., Konstan, J.A., Borchers, A., and Riedl, J., “An Algorithmic Framework for
Performing Collaborative Filtering”, Proc. 22nd ACM SIGIR Conference on Information Re-
trieval, pp. 230–237, 1999.
15. Hofmann, T., “Latent Semantic Models for Collaborative Filtering”, ACM Transactions on
Information Systems 22 (2004), 89–115.
16. Kim, D., and Yum, B., “Collaborative Filtering Based on Iterative Principal Component Anal-
ysis”, Expert Systems with Applications 28 (2005), 823–830.
17. Koren, Y., “Factorization Meets the Neighborhood: a Multifaceted Collaborative Filtering
Model”, Proc. 14th ACM SIGKDD International Conference on Knowledge Discovery and
Data Mining, 2008.
18. Koren, Y., “Collaborative Filtering with Temporal Dynamics.” Proc. 15th ACM SIGKDD In-
ternational Conference on Knowledge Discovery and Data Mining, pp. 447–456, 2009.
19. Koren, Y., “Factor in the Neighbors: Scalable and Accurate Collaborative Filtering ”, ACM
Transactions on Knowledge Discovery from Data (TKDD),4(2010):1-24.
20. Linden, G., Smith, B., and York, J., “Amazon.com Recommendations: Item-to-Item Collabo-
rative Filtering”, IEEE Internet Computing 7 (2003), 76–80.
21. Marlin, B.M., Zemel, R.S., Roweis, S., and Slaney, M., “Collaborative Filtering and the Miss-
ing at Random Assumption”, Proc. 23rd Conference on Uncertainty in Artificial Intelligence,
2007.
22. Oard, D.W.,, and Kim, J., “Implicit Feedback for Recommender Systems”, Proc. 5th DELOS
Workshop on Filtering and Collaborative Filtering, pp. 31–36, 1998.
23. Paterek, A., “Improving Regularized Singular Value Decomposition for Collaborative Filter-
ing”, Proc. KDD Cup and Workshop, 2007.
186 Yehuda Koren and Robert Bell
24. Salakhutdinov, R., Mnih, A., and Hinton, G., “Restricted Boltzmann Machines for Collabora-
tive Filtering”, Proc. 24th Annual International Conference on Machine Learning, pp. 791–
798, 2007.
25. Salakhutdinov, R., and Mnih, A., “Probabilistic Matrix Factorization”, Advances in Neural
Information Processing Systems 20 (NIPS’07), pp. 1257–1264, 2008.
26. Sarwar, B.M., Karypis, G., Konstan, J.A., and Riedl, J., “Application of Dimensionality Re-
duction in Recommender System – A Case Study”, WEBKDD’2000.
27. Sarwar, B., Karypis, G., Konstan, J., and Riedl, J., “Item-based Collaborative Filtering Rec-
ommendation Algorithms”, Proc. 10th International Conference on the World Wide Web, pp.
285-295, 2001.
28. Takács G., Pilászy I., Németh B. and Tikk, D., “Major Components of the Gravity Recom-
mendation System”, SIGKDD Explorations 9 (2007), 80–84.
29. Takács G., Pilászy I., Németh B. and Tikk, D., “Matrix Factorization and Neighbor based Al-
gorithms for the Netflix Prize Problem”, Proc. 2nd ACM conference on Recommender Systems
(RecSys’08), pp.267–274, 2008.
30. Tintarev, N., and Masthoff, J., “A Survey of Explanations in Recommender Systems”,
ICDE’07 Workshop on Recommender Systems and Intelligent User Interfaces, 2007.
31. Toscher, A., Jahrer, M., and Legenstein, R., “Improved Neighborhood-Based Algorithms for
Large-Scale Recommender Systems”, KDD’08 Workshop on Large Scale Recommenders Sys-
tems and the Netflix Prize, 2008.
32. Wang, J., de Vries, A.P., and Reinders, M.J.T, “Unifying User-based and Item-based Collab-
orative Filtering Approaches by Similarity Fusion”, Proc. 29th ACM SIGIR Conference on
Information Retrieval, pp. 501–508, 2006.