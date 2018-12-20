Chapter 2
Data Mining Methods for Recommender
Systems
Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
Abstract In this chapter, we give an overview of the main Data Mining techniques
used in the context of Recommender Systems. We first describe common prepro-
cessing methods such as sampling or dimensionality reduction. Next, we review the
most important classification techniques, including Bayesian Networks and Support
Vector Machines. We describe the k-means clustering algorithm and discuss several
alternatives. We also present association rules and related algorithms for an effi-
cient training process. In addition to introducing these techniques, we survey their
uses in Recommender Systems and present cases where they have been successfully
applied.
2.1 Introduction
Recommender Systems (RS) typically apply techniques and methodologies from
other neighboring areas – such as Human Computer Interaction (HCI) or Informa-
tion Retrieval (IR). However, most of these systems bear in their core an algorithm
that can be understood as a particular instance of a Data Mining (DM) technique.
The process of data mining typically consists of 3 steps, carried out in succes-
sion: Data Preprocessing [59], Data Analysis, and Result Interpretation (see Figure
2.1). We will analyze some of the most important methods for data preprocessing
in Section 2.2. In particular, we will focus on sampling, dimensionality reduction,
and the use of distance functions because of their significance and their role in RS.
In Sections 2.3 through 2.5, we provide an overview introduction to the data mining
methods that are most commonly used in RS: classification, clustering and associa-
Xavier Amatriain
Telefonica Research, Via Augusta, 122, Barcelona 08021, Spain e-mail: xar@tid.es
Alejandro Jaimes
Yahoo! Research, Av.Diagonal, 177, Barcelona 08018, Spain. Work on the chapter was performed
while the author was at Telefonica Research. e-mail: ajaimes@yahoo-inc.com
Nuria Oliver
Telefonica Research, Via Augusta, 122, Barcelona 08021, Spain e-mail: nuriao@tid.es
Josep M. Pujol
Telefonica Research, Via Augusta, 122, Barcelona 08021, Spain e-mail: jmps@tid.es
F. Ricci et al. (eds.), Recommender Systems Handbook,
DOI 10.1007/978-0-387-85820-3_2, © Springer Science+Business Media, LLC 2011
39
40 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
tion rule discovery (see Figure 2.1 for a detailed view of the different topics covered
in the chapter).
Fig. 2.1: Main steps and methods in a Data Mining problem, with their correspon-
dence to chapter sections.
This chapter does not intend to give a thorough review of Data Mining methods,
but rather to highlight the impact that DM algorithms have in the RS field, and to
provide an overview of the key DM techniques that have been successfully used.
We shall direct the interested reader to Data Mining textbooks (see [28, 73], for
example) or the more focused references that are provided throughout the chapter.
2.2 Data Preprocessing
We define data as a collection of objects and their attributes, where an attribute is
defined as a property or characteristic of an object. Other names for object include
record, item, point, sample, observation, or instance. An attribute might be also be
referred to as a variable, field, characteristic, or feature.
2 Data Mining Methods for Recommender Systems 41
Real-life data typically needs to be preprocessed (e.g. cleansed, filtered, trans-
formed) in order to be used by the machine learning techniques in the analysis step.
In this section, we focus on three issues that are of particular importance when de-
signing a RS. First, we review different similarity or distance measures. Next, we
discuss the issue of sampling as a way to reduce the number of items in very large
collections while preserving its main characteristics. Finally, we describe the most
common techniques to reduce dimensionality.
2.2.1 Similarity Measures
One of the preferred approaches to collaborative filtering (CF) recommenders is to
use the kNN classifier that will be described in Section 2.3.1. This classification
method – as most classifiers and clustering techniques – is highly dependent on
defining an appropriate similarity or distance measure.
The simplest and most common example of a distance measure is the Euclidean
distance:
d(x,y) =
√
n
∑
k=1
(x k −y k ) 2 (2.1)
where n is the number of dimensions (attributes) and x k and y k are the k th attributes
(components) of data objects x and y, respectively.
The Minkowski Distance is a generalization of Euclidean Distance:
d(x,y) = (
n
∑
k=1
|x k −y k | r )
1
r
(2.2)
where r is the degree of the distance. Depending on the value of r, the generic
Minkowski distance is known with specific names: For r = 1, the city block, (Man-
hattan, taxicab or L1 norm) distance; For r = 2, the Euclidean distance; For r → ∞,
the supremum (L max norm or L ∞ norm) distance, which corresponds to computing
the maximum difference between any dimension of the data objects.
The Mahalanobis distance is defined as:
d(x,y) =
√
(x−y) σ −1 (x−y) T (2.3)
where σ is the covariance matrix of the data.
Another very common approach is to consider items as document vectors of an
n-dimensional space and compute their similarity as the cosine of the angle that they
form:
cos(x,y) =
(x•y)
||x||||y||
(2.4)
42 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
where • indicates vector dot product and ||x|| is the norm of vector x. This similarity
is known as the cosine similarity or the L2 Norm .
The similarity between items can also be given by their correlation which mea-
sures the linear relationship between objects. While there are several correlation co-
efficients that may be applied, the Pearson correlation is the most commonly used.
Given the covariance of data points x and y Σ , and their standard deviation σ , we
compute the Pearson correlation using:
Pearson(x,y) =
Σ (x,y)
σ x × σ y
(2.5)
RS have traditionally used either the cosine similarity (Eq. 2.4) or the Pearson
correlation (Eq. 2.5) – or one of their many variations through, for instance, weight-
ing schemes – both Chapters 5 and 4 detail the use of different distance functions
for CF However, most of the other distance measures previously reviewed are pos-
sible. Spertus et al. [69] did a large-scale study to evaluate six different similarity
measures in the context of the Orkut social network. Although their results might be
biased by the particular setting of their experiment, it is interesting to note that the
best response to recommendations were to those generated using the cosine similar-
ity. Lathia et al. [48] also carried out a study of several similarity measures where
they concluded that, in the general case, the prediction accuracy of a RS was not af-
fected by the choice of the similarity measure. As a matter of fact and in the context
of their work, using a random similarity measure sometimes yielded better results
than using any of the well-known approaches.
Finally, several similarity measures have been proposed in the case of items that
only have binary attributes. First, the M01, M10, M11, and M00 quantities are com-
puted, where M01 = the number of attributes where x was 0 and y was 1, M10 =
the number of attributes where x was 1 and y was 0, and so on. From those quan-
tities we can compute: The Simple Matching coefficient SMC =
numberofmatches
numberofattributes
=
M11+M00
M01+M10+M00+M11 ; the Jaccard coefficient JC =
M11
M01+M10+M11 . The Extended Jac-
card (Tanimoto) coefficient, a variation of JC for continuous or count attributes that
is computed by d =
x•y
∥x∥ 2 +∥x∥ 2 −x•y .
2.2.2 Sampling
Sampling is the main technique used in DM for selecting a subset of relevant data
from a large data set. It is used both in the preprocessing and final data interpretation
steps. Sampling may be used because processing the entire data set is computation-
ally too expensive. It can also be used to create training and testing datasets. In this
case, the training dataset is used to learn the parameters or configure the algorithms
used in the analysis step, while the testing dataset is used to evaluate the model or
configuration obtained in the training phase, making sure that it performs well (i.e.
generalizes) with previously unseen data.
2 Data Mining Methods for Recommender Systems 43
The key issue to sampling is finding a subset of the original data set that is repre-
sentative – i.e. it has approximately the same property of interest – of the entire set.
The simplest sampling technique is random sampling, where there is an equal prob-
ability of selecting any item. However, more sophisticated approaches are possible.
For instance, in stratified sampling the data is split into several partitions based on
a particular feature, followed by random sampling on each partition independently.
The most common approach to sampling consists of using sampling without re-
placement: When an item is selected, it is removed from the population. However, it
is also possible to perform sampling with replacement, where items are not removed
from the population once they have been selected, allowing for the same sample to
be selected more than once.
It is common practice to use standard random sampling without replacement with
an 80/20 proportion when separating the training and testing data sets. This means
that we use random sampling without replacement to select 20% of the instances
for the testing set and leave the remaining 80% for training. The 80/20 proportion
should be taken as a rule of thumb as, in general, any value over 2/3 for the training
set is appropriate.
Sampling can lead to an over-specialization to the particular division of the train-
ing and testing data sets. For this reason, the training process may be repeated sev-
eral times. The training and test sets are created from the original data set, the model
is trained using the training data and tested with the examples in the test set. Next,
different training/test data sets are selected to start the training/testing process again
that is repeated K times. Finally, the average performance of the K learned mod-
els is reported. This process is known as cross-validation. There are several cross-
validation techniques. In repeated random sampling, a standard random sampling
process is carried out K times. In n-Fold cross validation, the data set is divided into
n folds. One of the folds is used for testing the model and the remaining n−1 folds
are used for training. The cross validation process is then repeated n times with each
of the n subsamples used exactly once as validation data. Finally, the leave-one-out
(LOO) approach can be seen as an extreme case of n-Fold cross validation where
n is set to the number of items in the data set. Therefore, the algorithms are run
as many times as data points using only one of them as a test each time. It should
be noted, though, that as Isaksson et al. discuss in [44], cross-validation may be
unreliable unless the data set is sufficiently large.
A common approach in RS is to sample the available feedback from the users –
e.g. in the form of ratings – to separate it into training and testing. Cross-validation
is also common. Although a standard random sampling is acceptable in the general
case, in others we might need to bias our sampling for the test set in different ways.
We might, for instance, decide to sample only from most recent ratings – since
those are the ones we would be predicting in a real-world situation. We might also
be interested in ensuring that the proportion of ratings per user is preserved in the
test set and therefore impose that the random sampling is done on a per user basis.
However, all these issues relate to the problem of evaluating RS, which is still a
matter of research and discussion.
44 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
2.2.3 Reducing Dimensionality
It is common in RS to have not only a data set with features that define a high-
dimensional space, but also very sparse information in that space – i.e. there are
values for a limited number of features per object. The notions of density and dis-
tance between points, which are critical for clustering and outlier detection, become
less meaningful in highly dimensional spaces. This is known as the Curse of Di-
mensionality. Dimensionality reduction techniques help overcome this problem by
transforming the original high-dimensional space into a lower-dimensionality.
Sparsity and the curse of dimensionality are recurring problems in RS. Even in
the simplest setting, we are likely to have a sparse matrix with thousands of rows
and columns (i.e. users and items), most of which are zeros. Therefore, dimension-
ality reduction comes in naturally. Applying dimensionality reduction makes such
a difference and its results are so directly applicable to the computation of the pre-
dicted value, that this is now considered to be an approach to RS design, rather than
a preprocessing technique.
In the following, we summarize the two most relevant dimensionality reduction
algorithms in the context of RS: Principal Component Analysis (PCA) and Singu-
lar Value Decomposition (SVD). These techniques can be used in isolation or as a
preprocessing step for any of the other techniques reviewed in this chapter.
2.2.3.1 Principal Component Analysis
Principal Component Analysis (PCA) [45] is a classical statistical method to find
patterns in high dimensionality data sets. PCA allows to obtain an ordered list of
components that account for the largest amount of the variance from the data in
terms of least square errors: The amount of variance captured by the first component
is larger than the amount of variance on the second component and so on. We can
reduce the dimensionality of the data by neglecting those components with a small
contribution to the variance.
Figure 2.2 shows the PCA analysis to a two-dimensional point cloud generated
by a combination of Gaussians. After the data is centered, the principal components
are obtained and denoted by u 1 and u 2 . Note that the length of the new coordi-
nates is relative to the energy contained in their eigenvectors. Therefore, for the
particular example depicted in Fig 2.2, the first component u 1 accounts for 83.5%
of the energy, which means that removing the second component u 2 would imply
losing only 16.5% of the information. The rule of thumb is to choose m ′ so that the
cumulative energy is above a certain threshold, typically 90%. PCA allows us to re-
trieve the original data matrix by projecting the data onto the new coordinate system
X ′
n×m ′
= X n×m W ′ m×m ′ . The new data matrix X ′ contains most of the information
of the original X with a dimensionality reduction of m−m ′ .
PCA is a powerful technique, but it does have important limitations. PCA relies
on the empirical data set to be a linear combination of a certain basis – although
generalizations of PCA for non-linear data have been proposed. Another important
2 Data Mining Methods for Recommender Systems 45
−4 −2 2 4
−2
−1
1
2
3
4
u 2
u 1
Fig. 2.2: PCA analysis of a two-dimensional point cloud from a combination of
Gaussians.TheprincipalcomponentsderivedusingPCSareu 1 andu 2 ,whoselength
is relative to the energy contained in the components.
assumption of PCA is that the original data set has been drawn from a Gaussian
distribution. When this assumption does not hold true, there is no warranty that the
principal components are meaningful.
Although current trends seem to indicate that other matrix factorizations tech-
niques such as SVD or Non-Negative Matrix Factorization are preferred, earlier
works used PCA. Goldberg et al. proposed an approach to use PCA in the context
ofanonlinejokerecommendationsystem[37].Theirsystem,knownasEigentaste
1 ,
startsfromastandardmatrixofuserratingstoitems.Theythenselecttheirgaugeset
by choosing the subset of items for which all users had a rating. This new matrix is
then used to compute the global correlation matrix where a standard 2-dimensional
PCA is applied.
2.2.3.2 Singular Value Decomposition
Singular Value Decomposition [38] is a powerful technique for dimensionality re-
duction. It is a particular realization of the Matrix Factorization approach and it is
therefore also related to PCA. The key issue in an SVD decomposition is to find a
lower dimensional feature space where the new features represent “concepts” and
the strength of each concept in the context of the collection is computable. Be-
cause SVD allows to automatically derive semantic “concepts” in a low dimensional
1
http://eigentaste.berkeley.edu
46 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
space, it can be used as the basis of latent-semantic analysis[24], a very popular
technique for text classification in Information Retrieval .
The core of the SVD algorithm lies in the following theorem: It is always possi-
ble to decompose a given matrix A into A =U λ V T . Given the n×m matrix data A
(n items, m features), we can obtain an n×r matrixU (n items, r concepts), an r×r
diagonal matrix λ (strength of each concept), and an m×r matrix V (m features, r
concepts). Figure 2.3 illustrates this idea. The λ diagonal matrix contains the sin-
gular values, which will always be positive and sorted in decreasing order. The U
matrix is interpreted as the “item-to-concept” similarity matrix, while the V matrix
is the “term-to-concept” similarity matrix.
A n
m
=
U
r
(items)
(features)
(concepts)
X
r
r X V
m
n
(items)
(features)
r
(concepts)
λ
Fig. 2.3: Illustrating the basic Singular Value Decomposition Theorem: an item ×
features matrix can be decomposed into three different ones: an item × concepts, a
concept strength, and a concept × features.
In order to compute the SVD of a rectangular matrix A, we consider AA T and
A T A. The columns of U are the eigenvectors of AA T , and the columns of V are
the eigenvectors of A T A. The singular values on the diagonal of λ are the positive
square roots of the nonzero eigenvalues of both AA T and A T A. Therefore, in order
to compute the SVD of matrix A we first compute T as AA T and D as A T A and then
compute the eigenvectors and eigenvalues for T and D.
The r eigenvalues in λ are ordered in decreasing magnitude. Therefore, the orig-
inal matrix A can be approximated by simply truncating the eigenvalues at a given k.
The truncated SVD creates a rank-k approximation to A so that A k =U k λ k V T
k
. A k is
the closest rank-k matrix to A. The term “closest” means that A k minimizes the sum
of the squares of the differences of the elements of A and A k . The truncated SVD is
a representation of the underlying latent structure in a reduced k-dimensional space,
which generally means that the noise in the features is reduced.
The use of SVD as tool to improve collaborative filtering has been known for
some time. Sarwar et al. [66] describe two different ways to use SVD in this context.
First, SVD can be used to uncover latent relations between customers and products.
In order to accomplish this goal, they first fill the zeros in the user-item matrix
with the item average rating and then normalize by subtracting the user average.
This matrix is then factored using SVD and the resulting decomposition can be
used – after some trivial operations – directly to compute the predictions. The other
2 Data Mining Methods for Recommender Systems 47
approach is to use the low-dimensional space resulting from the SVD to improve
neighborhood formation for later use in a kNN approach.
As described by Sarwar et al.[65], one of the big advantages of SVD is that there
are incremental algorithms to compute an approximated decomposition. This allows
to accept new users or ratings without having to recompute the model that had been
built from previously existing data. The same idea was later extended and formal-
ized by Brand [14] into an online SVD model. The use of incremental SVD methods
has recently become a commonly accepted approach after its success in the Netflix
Prize
2 . The publication of Simon Funk’s simplified incremental SVD method [35]
marked an inflection point in the contest. Since its publication, several improve-
ments to SVD have been proposed in this same context (see Paterek’s ensembles of
SVD methods [56] or Kurucz et al. evaluation of SVD parameters [47]).
Finally, it should be noted that different variants of Matrix Factorization (MF)
methods such as the Non-negative Matrix Factorization (NNMF) have also been
used[74]. These algorithms are, in essence, similar to SVD. The basic idea is to
decompose the ratings matrix into two matrices, one of which contains features
that describe the users and the other contains features describing the items. Matrix
Factorization methods are better than SVD at handling the missing values by in-
troducing a bias term to the model. However, this can also be handled in the SVD
preprocessing step by replacing zeros with the item average. Note that both SVD
and MF are prone to overfitting. However, there exist MF variants, such as the Reg-
ularized Kernel Matrix Factorization, that can avoid the issue efficiently. The main
issue with MF – and SVD – methods is that it is unpractical to recompute the fac-
torization every time the matrix is updated because of computational complexity.
However, Rendle and Schmidt-Thieme [62] propose an online method that allows
to update the factorized approximation without recomputing the entire model.
Chapter 5 details the use of SVD and MF in the context of the Netflix Prize and
is therefore a good complement to this introduction.
2.2.4 Denoising
Data collected for data-mining purposes might be subject to different kinds of noise
such as missing values or outliers. Denoising is a very important preprocessing step
that aims at removing any unwanted effect in the data while maximizing its infor-
mation.
In a general sense we define noise as any unwanted artifact introduced in the data
collection phase that might affect the result of our data analysis and interpretation.
In the context of RS, we distinguish between natural and malicious noise [55]. The
formerreferstonoisethatisunvoluntarelyintroducedbyuserswhengivingfeedback
on their preferences. The latter refers to noise that is deliberately introduced in a
system in order to bias the results.
2
http://www.netflixprize.com
48 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
It is clear that malicious noise can affect the output of a RS. But, also, we per-
formed a study that concluded that the effects of natural noise on the performance
of RS is far from being negligible [4]. In order to address this issue, we designed
a denoising approach that is able to improve accuracy by asking some users to re-
rate some items [5]. We concluded that accuracy improvements by investing in this
pre-processing step could be larger than the ones obtained by complex algorithm
optimizations.
2.3 Classification
A classifier is a mapping between a feature space and a label space, where the fea-
tures represent characteristics of the elements to classify and the labels represent
the classes. A restaurant RS, for example, can be implemented by a classifier that
classifies restaurants into one of two categories (good, bad) based on a number of
features that describe it.
There are many types of classifiers, but in general we will talk about either su-
pervised or unsupervised classification. In supervised classification, a set of labels
or categories is known in advance and we have a set of labeled examples which
constitute a training set. In unsupervised classification, the labels or categories are
unknown in advance and the task is to suitably (according to some criteria) organize
the elements at hand. In this section we describe several algorithms to learn super-
vised classifiers and will be covering unsupervised classification (i.e. clustering) in
Sec. 2.4.
2.3.1 Nearest Neighbors
Instance-based classifiers work by storing training records and using them to pre-
dict the class label of unseen cases. A trivial example is the so-called rote-learner.
This classifier memorizes the entire training set and classifies only if the attributes
of the new record match one of the training examples exactly. A more elaborate, and
far more popular, instance-based classifier is the Nearest neighbor classifier (kNN)
[22]. Given a point to be classified, the kNN classifier finds the k closest points
(nearest neighbors) from the training records. It then assigns the class label accord-
ing to the class labels of its nearest-neighbors. The underlying idea is that if a record
falls in a particular neighborhood where a class label is predominant it is because
the record is likely to belong to that very same class.
Given a query point q for which we want to know its class l, and a training
set X = {{x 1 ,l 1 }...{x n }}, where x j is the j-th element and l j is its class label, the
k-nearest neighbors will find a subset Y = {{y 1 ,l 1 }...{y k }} such that Y ∈ X and
∑ k 1 d(q,y k ) is minimal. Y contains the k points in X which are closest to the query
point q. Then, the class label of q is l = f({l 1 ...l k }).
2 Data Mining Methods for Recommender Systems 49
−3 −2 −1 0 1 2 3
−3
−2
−1
0
1
2
3


items of cluster 1
items of cluster 2
item to classify
−3 −2 −1 0 1 2 3
−3
−2
−1
0
1
2
3


items of cluster 1
items of cluster 2
item to classify
?
Fig. 2.4: Example of k-Nearest Neighbors. The left subfigure shows the training
points with two class labels (circles and squares) and the query point (as a triangle).
The right sub-figure illustrates closest neighborhood for k = 1 and k = 7. The query
point would be classified as square for k = 1, and as a circle for k = 5 according to
the simple majority vote rule. Note that the query points was just on the boundary
between the two clusters.
Perhaps the most challenging issue in kNN is how to choose the value of k. If
k is too small, the classifier will be sensitive to noise points. But if k is too large,
the neighborhood might include too many points from other classes. The right plot
in Fig. 2.4 shows how different k yields different class label for the query point, if
k = 1 the class label would be circle whereas k = 7 classifies it as square. Note that
the query point from the example is on the boundary of two clusters, and therefore,
it is difficult to classify.
kNN classifiers are amongst the simplest of all machine learning algorithms.
Since kNN does not build models explicitly it is considered a lazy learner. Un-
like eager learners such as decision trees or rule-based systems (see 2.3.2 and 2.3.3,
respectively), kNN classifiers leave many decisions to the classification step. There-
fore, classifying unknown records is relatively expensive.
Nearest Neighbor is one of the most common approaches to CF – and therefore
to designing a RS. As a matter of fact, any overview on RS – such as the one by
Adomavicius and Tuzhilin [1] – will include an introduction to the use of nearest
neighbors in this context. One of the advantages of this classifier is that it is con-
ceptually very much related to the idea of CF: Finding like-minded users (or similar
items) is essentially equivalent to finding neighbors for a given user or an item. The
other advantage is that, being the kNN classifier a lazy learner, it does not require
to learn and maintain a given model. Therefore, in principle, the system can adapt
to rapid changes in the user ratings matrix. Unfortunately, this comes at the cost of
recomputing the neighborhoods and therefore the similarity matrix. This is why we
proposed a neighborhood model that uses a reduced set of experts as the source for
selecting neighbors [3].
50 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
The kNN approach, although simple and intuitive, has shown good accuracy re-
sults and is very amenable to improvements. As a matter of fact, its supremacy as
the de facto standard for CF recommendation has only been challenged recently by
approaches based on dimensionality reduction such as the ones reviewed in Section
2.2.3. That said, the traditional kNN approach to CF has experienced improvements
in several directions. For instance, in the context of the Netflix Prize, Bell and Ko-
ren propose a method to remove global effects such as the fact that some items may
attract users that consistently rate lower. They also propose an optimization method
for computing interpolating weights once the neighborhood is created.
See Chapters 5 and 4 for more details on enhanced CF techniques based on the
use of neighborhoods.
2.3.2 Decision Trees
Decision trees [61, 63] are classifiers on a target attribute (or class) in the form of a
tree structure. The observations (or items) to classify are composed of attributes and
their target value. The nodes of the tree can be: a) decision nodes, in these nodes a
single attribute-value is tested to determine to which branch of the subtree applies.
Or b) leaf nodes which indicate the value of the target attribute.
There are many algorithms for decision tree induction: Hunts Algorithm, CART,
ID3, C4.5, SLIQ, SPRINT to mention the most common. The recursive Hunt al-
gorithm, which is one of the earliest and easiest to understand, relies on the test
condition applied to a given attribute that discriminates the observations by their
target values. Once the partition induced by the test condition has been found, the
algorithm is recursively repeated until a partition is empty or all the observations
have the same target value.
Splits can be decided by maximizing the information gain, defined as follows,
∆ i = I(parent)−
k i
∑
j=1
N(v j )I(v j )
N
(2.6)
where k i are values of the attribute i, N is the number of observations, v j is the j-
th partition of the observations according to the values of attribute i. Finally, I is a
functionthatmeasuresnodeimpurity.Therearedifferentmeasuresofimpurity:Gini
Index, Entropy and misclassification error are the most common in the literature.
Decision tree induction stops once all observations belong to the same class (or
the same range in the case of continuous attributes). This implies that the impurity
of the leaf nodes is zero. For practical reasons, however, most decision trees imple-
mentations use pruning by which a node is no further split if its impurity measure
or the number of observations in the node are below a certain threshold.
The main advantages of building a classifier using a decision tree is that it is
inexpensive to construct and it is extremely fast at classifying unknown instances.
Another appreciated aspect of decision tree is that they can be used to produce a set
2 Data Mining Methods for Recommender Systems 51
of rules that are easy to interpret (see section 2.3.3) while maintaining an accuracy
comparable to other basic classification techniques.
Decision trees may be used in a model-based approach for a RS. One possibil-
ity is to use content features to build a decision tree that models all the variables
involved in the user preferences. Bouza et al. [12] use this idea to construct a Deci-
sion Tree using semantic information available for the items. The tree is built after
the user has rated only two items. The features for each of the items are used to
build a model that explains the user ratings. They use the information gain of every
feature as the splitting criteria. It should be noted that although this approach is in-
teresting from a theoretical perspective, the precision they report on their system is
worse than that of recommending the average rating.
As it could be expected, it is very difficult and unpractical to build a decision
tree that tries to explain all the variables involved in the decision making process.
Decision trees, however, may also be used in order to model a particular part of
the system. Cho et al. [18], for instance, present a RS for online purchases that
combines the use of Association Rules (see Section 2.5) and Decision Trees. The
Decision Tree is used as a filter to select which users should be targeted with recom-
mendations. In order to build the model they create a candidate user set by selecting
those users that have chosen products from a given category during a given time
frame. In their case, the dependent variable for building the decision tree is cho-
sen as whether the customer is likely to buy new products in that same category.
Nikovski and Kulev [54] follow a similar approach combining Decision Trees and
Association Rules. In their approach, frequent itemsets are detected in the purchase
dataset and then they apply standard tree-learning algorithms for simplifying the
recommendations rules.
Another option to use Decision Trees in a RS is to use them as a tool for item
ranking. The use of Decision Trees for ranking has been studied in several settings
and their use in a RS for this purpose is fairly straightforward [7, 17].
2.3.3 Ruled-based Classifiers
Rule-based classifiers classify data by using a collection of “if ... then ...” rules.
The rule antecedent or condition is an expression made of attribute conjunctions.
The rule consequent is a positive or negative classification.
We say that a rule r covers a given instance x if the attributes of the instance
satisfy the rule condition. We define the coverage of a rule as the fraction of records
that satisfy its antecedent. On the other hand, we define its accuracy as the fraction
of records that satisfy both the antecedent and the consequent. We say that a clas-
sifier contains mutually exclusive rules if the rules are independent of each other –
i.e. every record is covered by at most one rule. Finally we say that the classifier has
exhaustive rules if they account for every possible combination of attribute values
–i.e. each record is covered by at least one rule.
52 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
In order to build a rule-based classifier we can follow a direct method to extract
rules directly from data. Examples of such methods are RIPPER, or CN2. On the
other hand, it is common to follow an indirect method and extract rules from other
classification models such as decision trees or neural networks.
The advantages of rule-based classifiers are that they are extremely expressive
since they are symbolic and operate with the attributes of the data without any
transformation. Rule-based classifiers, and by extension decision trees, are easy to
interpret, easy to generate and they can classify new instances efficiently.
In a similar way to Decision Tress, however, it is very difficult to build a complete
recommender model based on rules. As a matter of fact, this method is not very
popular in the context of RS because deriving a rule-based system means that we
either have some explicit prior knowledge of the decision making process or that
we derive the rules from another model such a decision tree. However a rule-based
system can be used to improve the performance of a RS by injecting partial domain
knowledge or business rules. Anderson et al. [6], for instance, implemented a CF
music RS that improves its performance by applying a rule-based system to the
results of the CF process. If a user rates an album by a given artist high, for instance,
predicted ratings for all other albums by this artist will be increased.
Gutta et al. [29] implemented a rule-based RS for TV content. In order to do,
so they first derived a C4.5 Decision Tree that is then decomposed into rules for
classifying the programs. Basu et al. [9] followed an inductive approach using the
Ripper [20] system to learn rules from data. They report slightly better results when
using hybrid content and collaborative data to learn rules than when following a
pure CF approach.
2.3.4 Bayesian Classifiers
A Bayesian classifier [34] is a probabilistic framework for solving classification
problems. It is based on the definition of conditional probability and the Bayes the-
orem. The Bayesian school of statistics uses probability to represent uncertainty
about the relationships learned from the data. In addition, the concept of priors is
very important as they represent our expectations or prior knowledge about what the
true relationship might be. In particular, the probability of a model given the data
(posterior) is proportional to the product of the likelihood times the prior proba-
bility (or prior). The likelihood component includes the effect of the data while the
prior specifies the belief in the model before the data was observed.
Bayesian classifiers consider each attribute and class label as (continuous or dis-
crete) random variables. Given a record with N attributes (A 1 ,A 2 ,...,A N ), the goal
is to predict class C k by finding the value of C k that maximizes the posterior prob-
ability of the class given the data P(C k |A 1 ,A 2 ,...,A N ). Applying Bayes’ theorem,
P(C k |A 1 ,A 2 ,...,A N ) ∝ P(A 1 ,A 2 ,...,A N |C k )P(C k )
A particular but very common Bayesian classifier is the Naive Bayes Classifier.
In order to estimate the conditional probability, P(A 1 ,A 2 ,...,A N |C k ), a Naive Bayes
2 Data Mining Methods for Recommender Systems 53
Classifier assumes the probabilistic independence of the attributes – i.e. the pres-
enceorabsenceofaparticularattributeisunrelatedtothepresenceorabsenceofany
other.ThisassumptionleadstoP(A 1 ,A 2 ,...,A N |C k )=P(A 1 |C k )P(A 2 |C k )...P(A N |C k ).
The main benefits of Naive Bayes classifiers are that they are robust to isolated
noise points and irrelevant attributes, and they handle missing values by ignoring
the instance during probability estimate calculations. However, the independence
assumption may not hold for some attributes as they might be correlated. In this
case, the usual approach is to use the so-called Bayesian Belief Networks (BBN)
(or Bayesian Networks, for short). BBN’s use an acyclic graph to encode the de-
pendence between attributes and a probability table that associates each node to its
immediate parents. BBN’s provide a way to capture prior knowledge in a domain
using a graphical model. In a similar way to Naive Bayes classifiers, BBN’s handle
incomplete data well and they are quite robust to model overfitting.
Bayesian classifiers are particularly popular for model-based RS. They are often
used to derive a model for content-based RS. However, they have also been used
in a CF setting. Ghani and Fano [36], for instance, use a Naive Bayes classifier to
implement a content-based RS. The use of this model allows for recommending
products from unrelated categories in the context of a department store.
Miyahara and Pazzani [52] implement a RS based on a Naive Bayes classifier.
In order to do so, they define two classes: like and don’t like. In this context they
propose two ways of using the Naive Bayesian Classifier: The Transformed Data
Model assumes that all features are completely independent, and feature selection
is implemented as a preprocessing step. On the other hand, the Sparse Data Model
assumes that only known features are informative for classification. Furthermore, it
only makes use of data which both users rated in common when estimating proba-
bilities. Experiments show both models to perform better than a correlation-based
CF.
Pronk et al. [58] use a Bayesian Naive Classifier as the base for incorporating
user control and improving performance, especially in cold-start situations. In order
to do so they propose to maintain two profiles for each user: one learned from the
rating history, and the other explicitly created by the user. The blending of both
classifiers can be controlled in such a way that the user-defined profile is favored
at early stages, when there is not too much rating history, and the learned classifier
takes over at later stages.
In the previous section we mentioned that Gutta et al. [29] implemented a
rule-based approach in a TV content RS. Another of the approaches they tested
was a Bayesian classifier. They define a two-class classifier, where the classes are
watched/not watched. The user profile is then a collection of attributes together with
the number of times they occur in positive and negative examples. This is used to
compute prior probabilities that a show belongs to a particular class and the con-
ditional probability that a given feature will be present if a show is either positive
or negative. It must be noted that features are, in this case, related to both content
–i.e. genre – and contexts –i.e. time of the day. The posteriori probabilities for a new
show are then computed from these.
54 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
Breese et al. [15] implement a Bayesian Network where each node corresponds
to each item. The states correspond to each possible vote value. In the network, each
item will have a set of parent items that are its best predictors. The conditional prob-
ability tables are represented by decision trees. The authors report better results for
this model than for several nearest-neighbors implementations over several datasets.
Hierarchical Bayesian Networks have also been used in several settings as a way
to add domain-knowledge for information filtering [78]. One of the issues with hier-
archical Bayesian networks, however, is that it is very expensive to learn and update
the model when there are many users in it. Zhang and Koren [79] propose a varia-
tion over the standard Expectation-Maximization (EM) model in order to speed up
this process in the scenario of a content-based RS.
2.3.5 Artificial Neural Networks
An Artificial Neural Network (ANN) [81] is an assembly of inter-connected nodes
and weighted links that is inspired in the architecture of the biological brain. Nodes
in an ANN are called neurons as an analogy with biological neurons. These simple
functional units are composed into networks that have the ability to learn a classifi-
cation problem after they are trained with sufficient data.
Input Signals
Synaptic Weights
Summing Junction
Activation
Function
Output
Threshold
w k0
w k1
w k2
w kp
x 0
x 1
x 2
x p
∑
φ (•)
θ k
v k
Fig. 2.5: Perceptron model
The simplest case of an ANN is the perceptron model, illustrated in figure 2.5. If
we particularize the activation function ϕ to be the simple Threshold Function, the
2 Data Mining Methods for Recommender Systems 55
output is obtained by summing up each of its input value according to the weights
of its links and comparing its output against some threshold θ k . The output function
can be expressed using Eq. 2.7. The perceptron model is a linear classifier that has
a simple and efficient learning algorithm. But, besides the simple Threshold Func-
tion used in the Perceptron model, there are several other common choices for the
activation function such as sigmoid, tanh, or step functions.
y k =
{
1, if ∑x i w ki ≥ θ k
0, if ∑x i w ki < θ k
(2.7)
An ANN can have any number of layers. Layers in an ANN are classified into
three types: input, hidden, and output. Units in the input layer respond to data that
is fed into the network. Hidden units receive the weighted output from the input
units. And the output units respond to the weighted output from the hidden units
and generate the final output of the network. Using neurons as atomic functional
units, there are many possible architectures to put them together in a network. But,
the most common approach is to use the feed-forward ANN. In this case, signals are
strictly propagated in one way: from input to output.
The main advantages of ANN are that – depending on the activation function
– they can perform non-linear classification tasks, and that, due to their parallel
nature, they can be efficient and even operate if part of the network fails. The main
disadvantage is that it is hard to come up with the ideal network topology for a
given problem and once the topology is decided this will act as a lower bound for
the classification error. ANN’s belong to the class of sub-symbolic classifiers, which
means that they provide no semantics for inferring knowledge – i.e. they promote a
kind of black-box approach.
ANN’s can be used in a similar way as Bayesian Networks to construct model-
based RS’s. However, there is no conclusive study to whether ANN introduce any
performance gain. As a matter of fact, Pazzani and Billsus [57] did a comprehen-
sive experimental study on the use of several machine learning algorithms for web
site recommendation. Their main goal was to compare the simple naive Bayesian
Classifier with computationally more expensive alternatives such as Decision Trees
and Neural Networks. Their experimental results show that Decision Trees perform
significantly worse. On the other hand ANN and the Bayesian classifier performed
similarly. They conclude that there does not seem to be a need for nonlinear clas-
sifiers such as the ANN. Berka et al. [31] used ANN to build an URL RS for web
navigation. They implemented a content-independent system based exclusively on
trails – i.e. associating pairs of domain names with the number of people who tra-
versedthem.Inordertodosotheyusedfeed-forwardMultilayerPerceptronstrained
with the Backpropagation algorithm.
ANN can be used to combine (or hybridize) the input from several recommen-
dation modules or data sources. Hsu et al. [30], for instance, build a TV recom-
mender by importing data from four different sources: user profiles and stereo-
types; viewing communities; program metadata; and viewing context. They use the
back-propagation algorithm to train a three-layered neural network. Christakou and
56 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
Stafylopatis [19] also built a hybrid content-based CF RS. The content-based rec-
ommender is implemented using three neural networks per user, each of them cor-
responding to one of the following features: “kinds”, “stars”, and “synopsis”. They
trained the ANN using the Resilient Backpropagation method.
2.3.6 Support Vector Machines
The goal of a Support Vector Machine (SVM) classifier [23] is to find a linear hy-
perplane (decision boundary) that separates the data in such a way that the margin is
maximized. For instance, if we look at a two class separation problem in two dimen-
sions like the one illustrated in figure 2.6, we can easily observe that there are many
possibleboundarylinesto separatethetwoclasses.Each boundaryhasanassociated
margin. The rationale behind SVM’s is that if we choose the one that maximizes the
margin we are less likely to missclassify unknown items in the future.
Large Margin
Small Margin
Support Vectors
w•x+b = 0
w•x+b = 1
w•x+b= −1
Fig. 2.6: Different boundary decisions are possible to separate two classes in two
dimensions. Each boundary has an associated margin.
A linear separation between two classes is accomplished through the function
w•x+b = 0. We define a function that can classify items of being of class +1 or -1
as long as they are separated by some minimum distance from the class separation
function. The function is given by Eq. 2.8
f(x) =
{
1, if w•x+b ≥ 1
−1, if w•x+b ≤ −1
(2.8)
Margin =
2
∥w∥ 2
(2.9)
2 Data Mining Methods for Recommender Systems 57
Following the main rationale for SVM’s, we would like to maximize the margin
between the two classes, given by equation 2.9. This is in fact equivalent to mini-
mizing the inverse value L(w) =
∥w∥ 2
2
but subjected to the constraints given by f(x).
This is a constrained optimization problem and there are numerical approaches to
solve it (e.g., quadratic programming).
If the items are not linearly separable we can decide to turn the svm into a soft
margin classifier by introducing a slack variable. In this case the formula to mini-
mize is given by equation 2.10 subject to the new definition of f(x) in equation 2.11.
On the other hand, if the decision boundary is not linear we need to transform data
into a higher dimensional space . This is accomplished thanks to a mathematical
transformation known as the kernel trick. The basic idea is to replace the dot prod-
ucts in equation 2.8 by a kernel function. There are many different possible choices
for the kernel function such as Polynomial or Sigmoid. But the most common kernel
functions are the family of Radial Basis Function (RBF).
L(w) =
∥w∥ 2
2
+C
N
∑
i=1
ε (2.10)
f(x) =
{
1, if w•x+b ≥ 1− ε
−1, if w•x+b ≤ −1+ ε
(2.11)
Support Vector Machines have recently gained popularity for their performance
and efficiency in many settings. SVM’s have also shown promising recent results
in RS. Kang and Yoo [46], for instance, report on an experimental study that aims
at selecting the best preprocessing technique for predicting missing values for an
SVM-based RS. In particular, they use SVD and Support Vector Regression. The
Support Vector Machine RS is built by first binarizing the 80 levels of available user
preference data. They experiment with several settings and report best results for a
threshold of 32 – i.e. a value of 32 and less is classified as prefer and a higher value
as do not prefer. The user id is used as the class label and the positive and negative
values are expressed as preference values 1 and 2.
Xu and Araki [76] used SVM to build a TV program RS. They used informa-
tion from the Electronic Program Guide (EPG) as features. But in order to reduce
features they removed words with lowest frequencies. Furthermore, and in order to
evaluate different approaches, they used both the Boolean and the Term frequency -
inverse document frequency (TFIDF) weighting schemes for features. In the former,
0 and 1 are used to represent absence or presence of a term on the content. In the
latter, this is turned into the TFIDF numerical value.
Xia et al.[75] present different approaches to using SVM’s for RS in a CF set-
ting. They explore the use of Smoothing Support Vector Machines (SSVM). They
also introduce a SSVM-based heuristic (SSVMBH) to iteratively estimate missing
elements in the user-item matrix. They compute predictions by creating a classifier
for each user. Their experimental results report best results for the SSVMBH as
compared to both SSVM’s and traditional user-based and item-based CF. Finally,
58 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
Oku et al. [27] propose the use of Context-Aware Vector Machines (C-SVM) for
context-aware RS. They compare the use of standard SVM, C-SVM and an exten-
sion that uses CF as well as C-SVM. Their results show the effectiveness of the
context-aware methods for restaurant recommendations.
2.3.7 Ensembles of Classifiers
The basic idea behind the use of ensembles of classifiers is to construct a set of
classifiers from the training data and predict class labels by aggregating their pre-
dictions. Ensembles of classifiers work whenever we can assume that the classifiers
are independent. In this case we can ensure that the ensemble will produce results
that are in the worst case as bad as the worst classifier in the ensemble. Therefore,
combining independent classifiers of a similar classification error will only improve
results.
In order to generate ensembles, several approaches are possible. The two most
common techniques are Bagging and Boosting. In Bagging, we perform sampling
with replacement, building the classifier on each bootstrap sample. Each sample has
probability(1−
1
N )
N
ofbeingselected–notethatifN islargeenough,thisconverges
to 1−
1
e
≈ 0.623. In Boosting we use an iterative procedure to adaptively change
distribution of training data by focusing more on previously misclassified records.
Initially, all records are assigned equal weights. But, unlike bagging, weights may
change at the end of each boosting round: Records that are wrongly classified will
have their weights increased while records that are classified correctly will have
their weights decreased. An example of boosting is the AdaBoost algorithm.
The use of ensembles of classifiers is common practice in the RS field. As a
matter of fact, any hybridation technique [16] can be considered an ensemble as
it combines in one way or another several classifiers. An explicit example of this
is Tiemann and Pauws’ music recommender, in which they use ensemble learning
methods to combine a social and a content-base RS [70].
Experimental results show that ensembles can produce better results than any
classifier in isolation. Bell et al. [11], for instance, used a combination of 107 differ-
ent methods in their progress prize winning solution to the Netflix challenge. They
state that their findings show that it pays off more to find substantially different ap-
proaches rather than focusing on refining a particular technique. In order to blend
the results from the ensembles they use a linear regression approach and to derive
weights for each classifier, they partition the test dataset into 15 different bins and
derive unique coefficients for each of the bins. Different uses of ensembles in the
context of the Netflix prize can be tracked in other approaches such as in Schclar et
al.’s [67] or Toescher et al.’s [71].
The boosting approach has also been used in RS. Freund et al., for instance,
present an algorithm called RankBoost to combine preferences [32]. They apply the
algorithm to produce movie recommendations in a CF setting.
2 Data Mining Methods for Recommender Systems 59
2.3.8 Evaluating Classifiers
The most commonly accepted evaluation measure for RS is the Mean Average Error
or Root Mean Squared Error of the predicted interest (or rating) and the measured
one. These measures compute accuracy without any assumption on the purpose of
the RS. However, as McNee et al. point out [51], there is much more than accuracy
to deciding whether an item should be recommended. Herlocker et al. [42] provide
a comprehensive review of algorithmic evaluation approaches to RS. They suggest
that some measures could potentially be more appropriate for some tasks. However,
they are not able to validate the measures when evaluating the different approaches
empirically on a class of recommendation algorithms and a single set of data.
A step forward is to consider that the purpose of a “real” RS is to produce a top-N
list of recommendations and evaluate RS depending on how well they can classify
items as being recommendable. If we look at our recommendation as a classifica-
tion problem, we can make use of well-known measures for classifier evaluation
such as precision and recall. In the following paragraphs, we will review some of
these measures and their application to RS evaluation. Note however that learn-
ing algorithms and classifiers can be evaluated by multiple criteria. This includes
how accurately they perform the classification, their computational complexity dur-
ing training , complexity during classification, their sensitivity to noisy data, their
scalability, and so on. But in this section we will focus only on classification perfor-
mance.
In order to evaluate a model we usually take into account the following measures:
True Positives (TP): number of instances classified as belonging to class A that
truly belong to class A; True Negatives (TN): number of instances classified as not
belonging to class A and that in fact do not belong to class A; False Positives (FP):
number of instances classified as class A but that do not belong to class A; False
Negatives (FN): instances not classified as belonging to class v but that in fact do
belong to class A.
The most commonly used measure for model performance is its Accuracy de-
fined as the ratio between the instances that have been correctly classified (as be-
longing or not to the given class) and the total number of instances: Accuracy =
(TP+TN)/(TP+TN +FP+FN). However, accuracy might be misleading in
many cases. Imagine a 2-class problem in which there are 99,900 samples of class
A and 100 of class B. If a classifier simply predicts everything to be of class A,
the computed accuracy would be of 99.9% but the model performance is question-
able because it will never detect any class B examples. One way to improve this
evaluation is to define the cost matrix where we declare the cost of misclassifying
class B examples as being of class A. In real world applications different types of
errors may indeed have very different costs. For example, if the 100 samples above
correspond to defective airplane parts in an assembly line, incorrectly rejecting a
non-defective part (one of the 99,900 samples) has a negligible cost compared to
the cost of mistakenly classifying a defective part as a good part.
Other common measures of model performance, particularly in Information Re-
trieval, are Precision and Recall . Precision, defined as P = TP/(TP+FP), is a
60 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
measure of how many errors we make in classifying samples as being of class A.
On the other hand, recall, R = TP/(TP+FN), measures how good we are in not
leaving out samples that should have been classified as belonging to the class. Note
that these two measures are misleading when used in isolation in most cases. We
could build a classifier of perfect precision by not classifying any sample as being
of class A (therefore obtaining 0 TP but also 0 FP). Conversely, we could build a
classifier of perfect recall by classifying all samples as belonging to class A. As a
matter of fact, there is a measure, called the F 1 -measure that combines both Preci-
sion and Recall into a single measure as: F 1 =
2RP
R+P
=
2TP
2TP+FN+FP
Sometimes we would like to compare several competing models rather than es-
timate their performance independently. In order to do so we use a technique de-
veloped in the 1950s for analysis of noisy signals: the Receiver Operating Charac-
teristic (ROC) Curve. A ROC curve characterizes the relation between positive hits
and false alarms. The performance of each classifier is represented as a point on the
curve (see Fig. 2.7).
0
0.2
0.4
0.6
0.8
1
0 0.2 0.4 0.6 0.8 1
True Positive Rate
False Positive Rate
Model 1
Model 2
Fig. 2.7: Example of ROC Curve. Model 1 performs better for low False Positive
Rates while Model 2 is fairly consistent throughout and outperforms Model 1 for
False Positive Rates higher than 0.25
Ziegler et al. show [80] that evaluating recommender algorithms through top-N
lists measures still does not map directly to the user’s utility function. However, it
does address some of the limitations of the more commonly accepted accuracy mea-
sures, such as MAE. Basu et al. [10], for instance, use this approach by analyzing
which of the items predicted in the top quartile of the rating scale were actually
evaluated in the top quartile by the user. McLaughlin and Herlocker [50] propose
2 Data Mining Methods for Recommender Systems 61
a modified precision measure in which non-rated items are counted as not recom-
mendable. This precision measure in fact represents a lower-bound of the “real”
precision. Although the F-measure can be directly derived from the precision-recall
values, it is not common to find it in RS evaluations. Huang et al. [43] and Bozzon
et al. [13], and Miyahara and Pazzani [52] are some of the few examples of the use
of this measure.
ROC curves have also been used in evaluating RS. Zhang et al. [64] use the value
of the area under the ROC curve as their evaluation measure when comparing the
performance of different algorithms under attack. Banerjee and Ramanathan [8] also
use the ROC curves to compare the performance of different models.
It must be noted, though, that the choice of a good evaluation measure, even in
the case of a top-N RS, is still a matter of discussion. Many authors have proposed
measures that are only indirectly related to these traditional evaluation schemes.
Deshpande and Karypis [25], for instance, propose the use of the hit rate and the
average reciprocal hit-rank. On the other hand, Breese et al. [15] define a measure
of the utility of the recommendation in a ranked list as a function of the neutral vote.
Note that Chapter 8 details on the use of some of these evaluation measures in
the context of RS and is therefore a good place to continue if you are interested on
this topic.
2.4 Cluster Analysis
The main problem for scaling a CF classifier is the amount of operations involved in
computingdistances–forfindingthe bestk-nearestneighbors,forinstance.Apossi-
ble solution is, as we saw in section 2.2.3, to reduce dimensionality. But, even if we
reduce dimensionality of features, we might still have many objects to compute the
distance to. This is where clustering algorithms can come into play. The same is true
for content-based RS, where distances among objects are needed to retrieve simi-
lar ones. Clustering is sure to improve efficiency because the number of operations
is reduced. However, and unlike dimensionality reduction methods, it is unlikely
that it can help improve accuracy. Therefore, clustering must be applied with care
when designing a RS, measuring the compromise between improved efficiency and
a possible decrease in accuracy.
Clustering [41], also referred to as unsupervised learning, consists of assigning
items to groups so that the items in the same groups are more similar than items
in different groups: the goal is to discover natural (or meaningful) groups that exist
in the data. Similarity is determined using a distance measure, such as the ones
reviewed in 2.2.1. The goal of a clustering algorithm is to minimize intra-cluster
distances while maximizing inter-cluster distances.
There are two main categories of clustering algorithms: hierarchical and parti-
tional. Partitional clustering algorithms divide data items into non-overlapping clus-
ters such that each data item is in exactly one cluster. Hierarchical clustering algo-
62 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
rithms successively cluster items within found clusters, producing a set of nested
cluster organized as a hierarchical tree.
Many clustering algorithms try to minimize a function that measures the quality
of the clustering. Such a quality function is often referred to as the objective func-
tion, so clustering can be viewed as an optimization problem: the ideal clustering
algorithm would consider all possible partitions of the data and output the partition-
ingthatminimizesthequalityfunction.Butthecorrespondingoptimizationproblem
is NP hard, so many algorithms resort to heuristics (e.g., in the k-means algorithm
using only local optimization procedures potentially ending in local minima). The
main point is that clustering is a difficult problem for which finding optimal solu-
tions is often not possible. For that same reason, selection of the particular clustering
algorithm and its parameters (e.g., similarity measure) depend on many factors, in-
cluding the characteristics of the data. In the following paragraphs we describe the
k-means clustering algorithm and some of its alternatives.
2.4.1 k-Means
k-Means clustering is a partitioning method. The function partitions the data set of
N items into k disjoint subsets S j that contain N j items so that they are as close
to each other as possible according a given distance measure. Each cluster in the
partition is defined by its N j members and by its centroid λ j . The centroid for each
cluster is the point to which the sum of distances from all items in that cluster is
minimized. Thus, we can define the k-means algorithm as an iterative process to
minimize E = ∑ k 1 ∑ n∈S j d(x n , λ j ), where x n is a vector representing the n-th item,
λ j is the centroid of the item in S j and d is the distance measure. The k-means
algorithm moves items between clusters until E cannot be decreased further.
The algorithm works by randomly selecting k centroids. Then all items are as-
signed to the cluster whose centroid is the closest to them. The new cluster centroid
needs to be updated to account for the items who have been added or removed from
the cluster and the membership of the items to the cluster updated. This operation
continues until there are no further items that change their cluster membership. Most
of the convergence to the final partition takes place during the first iterations of the
algorithm, and therefore, the stopping condition is often changed to “until relatively
few points change clusters” in order to improve efficiency.
The basic k-means is an extremely simple and efficient algorithm. However, it
does have several shortcomings: (1) it assumes prior knowledge of the data in order
tochoosetheappropriatek ;(2)thefinalclustersareverysensitivetotheselectionof
the initial centroids; and (3), it can produce empty cluster. k-means also has several
limitations with regard to the data: it has problems when clusters are of differing
sizes, densities, and non-globular shapes; and it also has problems when the data
contains outliers.
Xue et al. [77] present a typical use of clustering in the context of a RS by em-
ploying the k-means algorithm as a pre-processing step to help in neighborhood for-
2 Data Mining Methods for Recommender Systems 63
mation. They do not restrict the neighborhood to the cluster the user belongs to but
rather use the distance from the user to different cluster centroids as a pre-selection
step for the neighbors. They also implement a cluster-based smoothing technique in
which missing values for users in a cluster are replaced by cluster representatives.
Their method is reported to perform slightly better than standard kNN-based CF. In
a similar way, Sarwar et al. [26] describe an approach to implement a scalable kNN
classifier. They partition the user space by applying the bisecting k-means algorithm
and then use those clusters as the base for neighborhood formation. They report a
decrease in accuracy of around 5% as compared to standard kNN CF. However, their
approach allows for a significant improvement in efficiency.
Connor and Herlocker [21] present a different approach in which, instead of
users, they cluster items. Using the Pearson Correlation similarity measure they try
out four different algorithms: average link hierarchical agglomerative [39], robust
clustering algorithm for categorical attributes (ROCK) [40], kMetis, and hMetis
3 .
Although clustering did improve efficiency, all of their clustering techniques yielded
worse accuracy and coverage than the non-partitioned baseline. Finally, Li et al.[60]
and Ungar and Foster [72] present a very similar approach for using k-means clus-
tering for solving a probabilistic model interpretation of the recommender problem.
2.4.2 Alternatives to k-means
Density-based clustering algorithms such as DBSCAN work by building up on the
definition of density as the number of points within a specified radius. DBSCAN,
for instance, defines three kinds of points: core points are those that have more than
a specified number of neighbors within a given distance; border points have fewer
than the specified number but belong to a core point neighborhood; and noise points
are those that are neither core or border. The algorithm iteratively removes noise
points and performs clustering on the remaining points.
Message-passing clustering algorithms are a very recent family of graph-based
clustering methods. Instead of considering an initial subset of the points as centers
and then iteratively adapt those, message-passing algorithms initially consider all
points as centers – usually known as exemplars in this context. During the algorithm
execution points, which are now considered nodes in a network, exchange messages
until clusters gradually emerge. Affinity Propagation is an important representative
of this family of algorithms [33] that works by defining two kinds of messages
between nodes: “responsibility”, which reflects how well-suited receiving point is
to serve as exemplar of the point sending the message, taking into account other
potential exemplars;and “availability”,which is sent from candidate exemplar to the
point and reflects how appropriate it would be for the point to choose the candidate
as its exemplar, taking into account support from other points that are choosing that
same exemplar. Affinity propagation has been applied, with very good results, to
3
http://www.cs.umn.edu/ karypis/metis
64 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
problems as different as DNA sequence clustering, face clustering in images, or text
summarization.
Finally, Hierarchical Clustering, produces a set of nested clusters organized as
a hierarchical tree (dendogram). Hierarchical Clustering does not have to assume
a particular number of clusters in advanced. Also, any desired number of clusters
can be obtained by selecting the tree at the proper level. Hierarchical clusters can
also sometimes correspond to meaningful taxonomies. Traditional hierarchical al-
gorithms use a similarity or distance matrix and merge or split one cluster at a time.
There are two main approaches to hierarchical clustering. In agglomerative hier-
archical clustering we start with the points as individual clusters and at each step,
merge the closest pair of clusters until only one cluster (or k clusters) are left. In
divisive hierarchical clustering we start with one, all-inclusive cluster, and at each
step, split a cluster until each cluster contains a point (or there are k clusters).
To the best of our knowledge, alternatives to k-means such as the previous have
not been applied to RS. The simplicity and efficiency of the k-means algorithm
shadows possible alternatives. It is not clear whether density-based or hierarchical
clustering approaches have anything to offer in the RS arena. On the other hand,
message-passing algorithms have been shown to be more efficient and their graph-
based paradigm can be easily translated to the RS problem. It is possible that we see
applications of these algorithms in the coming years.
2.5 Association Rule Mining
Association Rule Mining focuses on finding rules that will predict the occurrence of
an item based on the occurrences of other items in a transaction. The fact that two
items are found to be related means co-occurrence but not causality. Note that this
technique should not be confused with rule-based classifiers presented in Sec. 2.3.3.
We define an itemset as a collection of one or more items (e.g. (Milk, Beer,
Diaper)). A k-itemset is an itemset that contains k items. The frequency of a given
itemset is knownas support count (e.g. (Milk, Beer, Diaper) = 131). And thesupport
of the itemset is the fraction of transactions that contain it (e.g. (Milk, Beer, Diaper)
= 0.12). A frequent itemset is an itemset with a support that is greater or equal to a
minsup threshold. An association rule is an expression of the form X ⇒ Y, where
X and Y are itemsets. (e.g. Milk,Diaper ⇒ Beer). In this case the support of the
association rule is the fraction of transactions that have both X and Y. On the other
hand, the confidence of the rule is how often items in Y appear in transactions that
contain X.
Given a set of transactions T, the goal of association rule mining is to find
all rules having support ≥ minsupthreshold and confidence ≥ minconfthreshold.
The brute-force approach would be to list all possible association rules, compute
the support and confidence for each rule and then prune rules that do not satisfy
both conditions. This is, however, computationally very expensive. For this reason,
we take a two-step approach: (1) Generate all itemsets whose support ≥ minsup
2 Data Mining Methods for Recommender Systems 65
(Frequent Itemset Generation); (2) Generate high confidence rules from each fre-
quent itemset (Rule Generation)
Several techniques exist to optimize the generation of frequent itemsets. On a
broad sense they can be classified into those that try to minimize the number of can-
didates (M), those that reduce the number of transactions (N), and those that reduce
the number of comparisons (NM). The most common approach though, is to reduce
the number of candidates using the Apriori principle. This principle states that if
an itemset is frequent, then all of its subsets must also be frequent. This is verified
using the support measure because the support of an itemset never exceeds that of
its subsets. The Apriori algorithm is a practical implementation of the principle.
Given a frequent itemset L, the goal when generating rules is to find all non-
empty subsets that satisfy the minimum confidence requirement. If |L| = k, then
there are 2k2 candidate association rules. So, as in the frequent itemset generation,
we need to find ways to generate rules efficiently. For the Apriori Algorithm we can
generate candidate rules by merging two rules that share the same prefix in the rule
consequent.
The effectiveness of association rule mining for uncovering patterns and driving
personalizedmarketingdecisionshasbeenknownforasometime[2].However,and
although there is a clear relation between this method and the goal of a RS, theyhave
not become mainstream. The main reason is that this approach is similar to item-
based CF but is less flexible since it requires of an explicit notion of transaction –
e.g. co-occurrence of events in a given session. In the next paragraphs we present
some promising examples, some of which indicate that association rules still have
not had their last word.
Mobasher et al. [53] present a system for web personalization based on associ-
ation rules mining. Their system identifies association rules from pageviews co-
occurrences based on users navigational patterns. Their approach outperforms a
kNN-based recommendation system both in terms of precision and coverage. Smyth
et al. [68] present two different case studies of using association rules for RS. In the
first case they use the a priori algorithm to extract item association rules from user
profiles in order to derive a better item-item similarity measure. In the second case,
they apply association rule mining to a conversational recommender. The goal here
is to find co-occurrent critiques – i.e. user indicating a preference over a particular
feature of the recommended item. Lin et al. [49] present a new association mining
algorithm that adjusts the minimum support of the rules during mining in order to
obtain an appropriate number of significant rule therefore addressing some of the
shortcomings of previous algorithms such as the a priori. They mine both associa-
tion rules between users and items. The measured accuracy outperforms previously
reported values for correlation-based recommendation and is similar to the more
elaborate approaches such as the combination of SVD and ANN.
Finally, as already mentioned in section 2.3.2, Cho et al. [18] combine Decision
Trees and Association Rule Mining in a web shop RS. In their system, associa-
tion rules are derived in order to link related items. The recommendation is then
computed by intersecting association rules with user preferences. They look for as-
sociation rules in different transaction sets such as purchases, basket placement, and
66 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
click-through. They also use a heuristic for weighting rules coming from each of the
transaction sets. Purchase association rules, for instance, are weighted higher than
click-through association rules.
2.6 Conclusions
This chapter has introduced the main data mining methods and techniques that can
be applied in the design of a RS. We have also surveyed their use in the literature
and provided some rough guidelines on how and where they can be applied.
We started by reviewing techniques that can be applied in the pre-processing
step. First, there is the choice of an appropriate distance measure, which is reviewed
in Section 2.2.1. This is required by most of the methods in the following steps.
The cosine similarity and Pearson correlation are commonly accepted as the best
choice. Although there have been many efforts devoted to improving these distance
measures, recent works seem to report that the choice of a distance function does not
play such an important role. Then, in Section 2.2.2, we reviewed the basic sampling
techniques that need to be applied in order to select a subset of an originally large
data set, or to separating a training and a testing set. Finally, we discussed the use
of dimensionality reduction techniques such as Principal Component Analysis and
Singular Value Decomposition in Section 2.2.3 as a way to address the curse of
dimensionality problem. We explained some success stories using dimensionality
reduction techniques, especially in the context of the Netflix prize.
In Section 2.3, we reviewed the main classification methods: namely, nearest-
neighbors, decision trees, rule-based classifiers, Bayesian networks, artificial neural
networks, and support vector machines. We saw that, although kNN ( see Section
2.3.1) CF is the preferred approach, all those classifiers can be applied in different
settings. Decision trees ( see Section 2.3.2) can be used to derive a model based
on the content of the items or to model a particular part of the system. Decision
rules ( see Section 2.3.3) can be derived from a pre-existing decision trees, or can
also be used to introduce business or domain knowledge. Bayesian networks ( see
Section 2.3.4) are a popular approach to content-based recommendation, but can
also be used to derive a model-based CF system. In a similar way, Artificial Neu-
ral Networks can be used to derive a model-based recommender but also to com-
bine/hybridize several algorithms. Finally, support vector machines ( see Section
2.3.6) are gaining popularity also as a way to infer content-based classifications or
derive a CF model.
Choosing the right classifier for a RS is not easy and is in many senses task and
data-dependent. In the case of CF, some results seem to indicate that model-based
approaches using classifiers such as the SVM or Bayesian Networks can slightly
improve performance of the standard kNN classifier. However, those results are non-
conclusive and hard to generalize. In the case of a content-based RS there is some
evidence that in some cases Bayesian Networks will perform better than simpler
2 Data Mining Methods for Recommender Systems 67
methods such as decision trees. However, it is not clear that more complex non-
linear classifiers such as the ANN or SVMs can perform better.
Therefore, the choice of the right classifier for a specific recommending task still
has nowadays much of exploratory. A practical rule-of-thumb is to start with the
simplest approach and only introduce complexity if the performance gain obtained
justifies it. The performance gain should of course balance different dimensions
such as prediction accuracy or computational efficiency.
We reviewed clustering algorithms in Section 2.4. Clustering is usually used in
RS to improve performance. A previous clustering step, either in the user of item
space, reduces the number of distance computations we need to perform. However,
this usually comes at the price of a lower accuracy so it should be handled with
care. As a matter of fact, improving efficiency by using a dimensionality reduction
technique such as SVD is probably a better choice in the general case. As opposed
to what happens with classifiers, not so many clustering algorithms have been used
in the context of RS. The simplicity and relative efficiency of the k-means algorithm
(see Section 2.4.1) make it hard to find a practical alternative. We reviewed some
of them such as Hierarchical Clustering or Message-passing algorithms in Section
2.4.2. Although these techniques have still not been applied for RS, they offer a
promising avenue for future research.
Finally, in Section 2.5, we described association rules and surveyed their use in
RS. Association rules offer an intuitive framework for recommending items when-
ever there is an explicit or implicit notion of transaction. Although there exist effi-
cient algorithms for computing association rules, and they have proved more accu-
rate than standard kNN CF, they are still not a favored approach.
The choice of the right DM technique in designing a RS is a complex task that
is bound by many problem-specific constraints. However, we hope that the short
review of techniques and experiences included in this chapter can help the reader
make a much more informed decision. Besides, we have also uncovered areas that
are open to many further improvements, and where there is still much exciting and
relevant research to be done in the coming years.
Acknowledgments
This chapter has been written with partial support of an ICREA grant from the
Generalitat de Catalunya.
References
1. Adomavicius, G.,and Tuzhilin, A., Toward the next generation of recommender systems: A
survey of the state-of-the-art and possible extensions. IEEE Transactions on Knowledge and
Data Engineering, 17(6):734–749, 2005.
68 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
2. Agrawal, R.,and Srikant, R., Fast algorithms for mining association rules in large databases.
In Proceedings of the 20th International Conference on Very Large Data Bases, 1994.
3. Amatriain, X., Lathia, N., Pujol, J.M., Kwak, H., and Oliver, N., The wisdom of the few: A
collaborative filtering approach based on expert opinions from the web. In Proc. of SIGIR ’09,
2009.
4. Amatriain, X., Pujol, J.M., and Oliver, N., I like it... i like it not: Evaluating user ratings noise
in recommender systems. In UMAP ’09, 2009.
5. Amatriain, X., Pujol, J.M., Tintarev, N., and Oliver, N., Rate it again: Increasing recommen-
dation accuracy by user re-rating. In Recys ’09, 2009.
6. Anderson,M.,Ball,M.,Boley,H.,Greene,S.,Howse,N.,Lemire,D.,andS.McGrath. Racofi:
A rule-applying collaborative filtering system. In Proc. IEEE/WIC COLA’03, 2003.
7. Baets, B.D., Growing decision trees in an ordinal setting. International Journal of Intelligent
Systems, 2003.
8. Banerjee, S.,and Ramanathan, K., Collaborative filtering on skewed datasets. In Proc. of
WWW ’08, 2008.
9. Basu, C., Hirsh, H., and Cohen, W., Recommendation as classification: Using social and
content-based information in recommendation. In In Proceedings of the Fifteenth National
Conference on Artificial Intelligence, pages 714–720. AAAI Press, 1998.
10. Basu, C., Hirsh, H., and Cohen, W., Recommendation as classification: Using social and
content-based information in recommendation. In AAAI Workshop on Recommender Systems,
1998.
11. Bell, R.M., Koren, Y., and Volinsky, C., The bellkor solution to the netflix prize. Technical
report, AT&T Labs Research, 2007.
12. Bouza, A., Reif, G., Bernstein, A., and Gall, H., Semtree: ontology-based decision tree algo-
rithm for recommender systems. In International Semantic Web Conference, 2008.
13. Bozzon, A., Prandi, G., Valenzise, G., and Tagliasacchi, M., A music recommendation sys-
tem based on semantic audio segments similarity. In Proceeding of Internet and Multimedia
Systems and Applications - 2008, 2008.
14. Brand, M., Fast online svd revisions for lightweight recommender systems. In SIAM Interna-
tional Conference on Data Mining (SDM), 2003.
15. Breese, J., Heckerman, D., and Kadie, C., Empirical analysis of predictive algorithms for
collaborative filtering. In Proceedings of the Fourteenth Annual Conference on Uncertainty in
Artificial Intelligence, page 4352, 1998.
16. Burke, R., Hybrid web recommender systems. pages 377–408. 2007.
17. Cheng, W., J. Hühn, and E. Hüllermeier. Decision tree and instance-based learning for label
ranking. In ICML ’09: Proceedings of the 26th Annual International Conference on Machine
Learning, pages 161–168, New York, NY, USA, 2009. ACM.
18. Cho, Y., Kim, J., and Kim, S., A personalized recommender system based on web usage
mining and decision tree induction. Expert Systems with Applications, 2002.
19. Christakou, C.,and Stafylopatis, A., A hybrid movie recommender system based on neural
networks. InISDA’05:Proceedingsof the5thInternationalConferenceonIntelligentSystems
Design and Applications, pages 500–505, 2005.
20. Cohen, W., Fast effective rule induction. In Machine Learning: Proceedings of the 12th
International Conference, 1995.
21. Connor, M.,and Herlocker, J., Clustering items for collaborative filtering. In SIGIR Workshop
on Recommender Systems, 2001.
22. Cover, T.,and Hart, P., Nearest neighbor pattern classification. Information Theory, IEEE
Transactions on, 13(1):21–27, 1967.
23. Cristianini, N.,and J. Shawe-Taylor. An Introduction to Support Vector Machines and Other
Kernel-based Learning Methods. Cambridge University Press, March 2000.
24. Deerwester, S., Dumais, S.T., Furnas, G.W., L. T. K., and Harshman, R., Indexing by latent
semantic analysis. Journal of the American Society for Information Science, 41, 1990.
25. Deshpande, M.,and Karypis, G., Item-based top-n recommendation algorithms. ACM Trans.
Inf. Syst., 22(1):143–177, 2004.
2 Data Mining Methods for Recommender Systems 69
26. B. S. et al. Recommender systems for large-scale e-commerce: Scalable neighborhood forma-
tion using clustering. In Proceedings of the Fifth International Conference on Computer and
Information Technology, 2002.
27. K. O. et al. Context-aware svm for context-dependent information recommendation. In Inter-
national Conference On Mobile Data Management, 2006.
28. P. T. et al. Introduction to Data Mining. Addison Wesley, 2005.
29. S. G. et al. Tv content recommender system. In AAAI/IAAI 2000, 2000.
30. S. H. et al. Aimed- a personalized tv recommendation system. In Interactive TV: a Shared
Experience, 2007.
31. T. B. et al. A trail based internet-domain recommender system using artificial neural networks.
In Proceedings of the Int. Conf. on Adaptive Hypermedia and Adaptive Web Based Systems,
2002.
32. Freund, Y., Iyer, R., Schapire, R.E., and Singer, Y., An efficient boosting algorithm for com-
bining preferences. Mach, J., Learn. Res., 4:933–969, 2003.
33. Frey, B.J.,and Dueck, D., Clustering by passing messages between data points. Science, 307,
2007.
34. Friedman, N., Geiger, D., and Goldszmidt, M., Bayesian network classifiers. Mach. Learn.,
29(2-3):131–163, 1997.
35. Funk, S., Netflix update: Try this at home, 2006.
36. Ghani, R.,and Fano, A., Building recommender systems using a knowledge base of product
semantics. In In 2nd International Conference on Adaptive Hypermedia and Adaptive Web
Based Systems, 2002.
37. Goldberg, K., Roeder, T., Gupta, D., and Perkins, C., Eigentaste: A constant time collaborative
filtering algorithm. Journal Information Retrieval, 4(2):133–151, July 2001.
38. Golub, G.,and Reinsch, C., Singular value decomposition and least squares solutions. Nu-
merische Mathematik, 14(5):403–420, April 1970.
39. Gose, E., Johnsonbaugh, R., and Jost, S., Pattern Recognition and Image Analysis. Prentice
Hall, 1996.
40. Guha, S., Rastogi, R., and Shim, K., Rock: a robust clustering algorithm for categorical at-
tributes. In Proc. of the 15th Intl Conf. On Data Eng., 1999.
41. Hartigan, J.A., Clustering Algorithms (Probability & Mathematical Statistics). John Wiley &
Sons Inc, 1975.
42. Herlocker,J.L., Konstan,J.A., Terveen,L.G., and Riedl, J.T., Evaluatingcollaborativefiltering
recommender systems. ACM Trans. Inf. Syst., 22(1):5–53, 2004.
43. Huang, Z., Zeng, D., and Chen, H., A link analysis approach to recommendation under sparse
data. In Proceedings of AMCIS 2004, 2004.
44. Isaksson, A., Wallman, M., H. Göransson, and Gustafsson, M.G., Cross-validation and boot-
strapping are unreliable in small sample classification. Pattern Recognition Letters, 29:1960–
1965, 2008.
45. Jolliffe, I.T., Principal Component Analysis. Springer, 2002.
46. Kang, H.,and Yoo, S., Svm and collaborative filtering-based prediction of user preference for
digital fashion recommendation systems. IEICE Transactions on Inf & Syst, 2007.
47. Kurucz, M., Benczur, A.A., and Csalogany, K., Methods for large scale svd with missing
values. In Proceedings of KDD Cup and Workshop 2007, 2007.
48. Lathia, N., Hailes, S., and Capra, L., The effect of correlation coefficients on communities of
recommenders. In SAC ’08: Proceedings of the 2008 ACM symposium on Applied computing,
pages 2000–2005, New York, NY, USA, 2008. ACM.
49. Lin, W.,and Alvarez, S., Efficient adaptive-support association rule mining for recommender
systems. Data Mining and Knowledge Discovery Journal, 6(1), 2004.
50. M. R. McLaughlin and Herlocker, J.L., A collaborative filtering algorithm and evaluation
metric that accurately model the user experience. In Proc. of SIGIR ’04, 2004.
51. S.M.McNee,Riedl,J.,andKonstan,J.A., Beingaccurateisnotenough:howaccuracymetrics
have hurt recommender systems. In CHI ’06: CHI ’06 extended abstracts on Human factors
in computing systems, pages 1097–1101, New York, NY, USA, 2006. ACM Press.
70 Xavier Amatriain, Alejandro Jaimes, Nuria Oliver, and Josep M. Pujol
52. Miyahara, K.,and Pazzani, M.J., Collaborative filtering with the simple bayesian classifier. In
Pacific Rim International Conference on Artificial Intelligence, 2000.
53. Mobasher, B., Dai, H., Luo, T., and Nakagawa, M., Effective personalization based on as-
sociation rule discovery from web usage data. In Workshop On Web Information And Data
Management, WIDM ’01, 2001.
54. Nikovski, D.,and Kulev, V., Induction of compact decision trees for personalized recommen-
dation. In SAC ’06: Proceedings of the 2006 ACM symposium on Applied computing, pages
575–581, New York, NY, USA, 2006. ACM.
55. M. P. Omahony. Detecting noise in recommender system databases. In In Proceedings of
the International Conference on Intelligent User Interfaces (IUI06), 29th1st, pages 109–115.
ACM Press, 2006.
56. Paterek, A., Improving regularized singular value decomposition for collaborative filtering.
In Proceedings of KDD Cup and Workshop 2007, 2007.
57. Pazzani, M.J.,and Billsus, D., Learning and revising user profiles: The identification of inter-
esting web sites. Machine Learning, 27(3):313–331, 1997.
58. Pronk, V., Verhaegh, W., Proidl, A., and Tiemann, M., Incorporating user control into rec-
ommender systems based on naive bayesian classification. In RecSys ’07: Proceedings of the
2007 ACM conference on Recommender systems, pages 73–80, 2007.
59. Pyle, D., Data Preparation for Data Mining. Morgan Kaufmann, second edition edition,
1999.
60. Li, B., K.Q., Clustering approach for hybrid recommender system. In Web Intelligence 03,
2003.
61. Quinlan, J.R., Induction of decision trees. Machine Learning, 1(1):81–106, March 1986.
62. Rendle, S.,and L. Schmidt-Thieme. Online-updating regularized kernel matrix factorization
models for large-scale recommender systems. In Recsys ’08: Proceedings of the 2008 ACM
conference on Recommender Systems, 2008.
63. Rokach, L., Maimon, O., Data Mining with Decision Trees: Theory and Applications, World
Scientific Publishing (2008).
64. Zhang, J., F.S., Ouyang, Y.,and Makedon, F., Analysis of a low-dimensional linear model
under recommendation attacks. In Proc. of SIGIR ’06, 2006.
65. Sarwar, B., Karypis, G., Konstan, J., and Riedl, J., Incremental svd-based algorithms for
highly scalable recommender systems. In 5th International Conference on Computer and
Information Technology (ICCIT), 2002.
66. Sarwar, B.M., Karypis, G., Konstan, J.A., and Riedl, J.T., Application of dimensionality re-
duction in recommender systemsa case study. In ACM WebKDD Workshop, 2000.
67. Schclar, A., Tsikinovsky, A., Rokach, L., Meisels, A., and Antwarg, L., Ensemble methods
for improving the performance of neighborhood-based collaborative filtering. In RecSys ’09:
Proceedings of the third ACM conference on Recommender systems, pages 261–264, New
York, NY, USA, 2009. ACM.
68. Smyth, B., K. McCarthy, Reilly, J., D. O‘Sullivan, L. McGinty, and Wilson, D., Case studies
in association rule mining for recommender systems. In Proc. of International Conference on
Artificial Intelligence (ICAI ’05), 2005.
69. Spertus, E., Sahami, M., and Buyukkokten, O., Evaluating similarity measures: A large-scale
study in the orkut social network. In Proceedings of the 2005 International Conference on
Knowledge Discovery and Data Mining (KDD-05), 2005.
70. Tiemann, M.,and Pauws, S., Towards ensemble learning for hybrid music recommendation.
In RecSys ’07: Proceedings of the 2007 ACM conference on Recommender systems, pages
177–178, New York, NY, USA, 2007. ACM.
71. Toescher, A., Jahrer, M., and Legenstein, R., Improved neighborhood-based algorithms for
large-scale recommender systems. In In KDD-Cup and Workshop 08, 2008.
72. Ungar, L.H.,and Foster, D.P., Clustering methods for collaborative filtering. In Proceedings
of the Workshop on Recommendation Systems, 2000.
73. Witten, I.H.,and Frank, E., Data Mining: Practical Machine Learning Tools and Techniques.
Morgan Kaufmann, second edition edition, 2005.
2 Data Mining Methods for Recommender Systems 71
74. Wu, M., Collaborative filtering via ensembles of matrix factorizations. In Proceedings of
KDD Cup and Workshop 2007, 2007.
75. Xia, Z., Dong, Y., and Xing, G., Support vector machines for collaborative filtering. In ACM-
SE 44: Proceedings of the 44th annual Southeast regional conference, pages 169–174, New
York, NY, USA, 2006. ACM.
76. Xu, J.,and Araki, K., A svm-based personal recommendation system for tv programs. In
Multi-Media Modelling Conference Proceedings, 2006.
77. Xue, G., Lin,R., Yang, C., Xi, Q., Zeng, W., H.-, Yu, J., and Chen, Z., Scalable collaborative
filtering using cluster-based smoothing. In Proceedings of the 2005 SIGIR, 2005.
78. Yu, K., Tresp, V., and Yu, S., A nonparametric hierarchical bayesian framework for informa-
tion filtering. In SIGIR ’04, 2004.
79. Zhang, Y.,and Koren, J., Efficient bayesian hierarchical user modeling for recommendation
system. In SIGIR 07, 2007.
80. Ziegler, C., McNee N., S. M., Konstan, J.A., and Lausen, G., Improving recommendation lists
through topic diversification. In Proc. of WWW ’05, 2005.
81. Zurada, J., Introduction to artificial neural systems. West Publishing Co., St. Paul, MN, USA,
1992.
