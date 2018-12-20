
# Chapter 2 Data Mining Methods for Recommender Systems

- Abstract

    In this chapter, we give an overview of the main Data Mining techniques used in the context of Recommender Systems. We first describe common preprocessing methods such as sampling or dimensionality reduction. Next, we review the most important classification techniques, including Bayesian Networks and Support Vector Machines. We describe the k-means clustering algorithm and discuss several alternatives. We also present association rules and related algorithms for an efficient training process. In addition to introducing these techniques, we survey their uses in Recommender Systems and present cases where they have been successfully applied.

he process of data mining typically consists of 3 steps, carried out in succession: Data Preprocessing[59], Data Analysis, and Result Interpretation (see Figure 2.1)
![image/fig-2-1.png](image/fig-2-1.png)


## 2.2 Data Preprocessing
We define data as a collection of objects and their attributes , where an attribute is defined as a property or characteristic of an object. Other names for object include record, item , point , sample , observation , or instance. An attribute might be also be
referred to as a variable, field , characteristic, or feature.

### 2.2.1 Similarity Measures

- (2.4)
$$cos(x,y) = \frac{(x \cdot y )}{ \begin{Vmatrix} x \end{Vmatrix} \begin{Vmatrix} y \end{Vmatrix} }   ............2.4$$

    where • indicates vector dot product and ||x|| is the norm of vector x. This similarity is known as the cosine imilarity or the L2 Norm

- (2.5) 皮尔斯相关系数

    The similarity between items can also be given by their correlation which measures the linear relationship between objects. While there are several correlation co-efficients that may be applied, the Pearson correlation is the most commonly used. Given the covariance of data points x and y Σ, and their standard deviation σ , we compute the Pearson correlation using:

$$Pearson(x,y)=\frac{\sum(x,y)}{\sigma _x \times \sigma _y}   ............2.5$$

```python
class CorrelationPearson(object):

    def average(self, x):
        assert len(x) > 0
        return float(sum(x)) / len(x)

    def result(self, x, y):
        assert len(x) == len(y)
        n = len(x)
        assert n > 0
        avg_x = self.average(x)
        avg_y = self.average(y)
        diffprod = 0
        xdiff2 = 0
        ydiff2 = 0
        for idx in range(n):
            xdiff = x[idx] - avg_x
            ydiff = y[idx] - avg_y
            diffprod += xdiff * ydiff
            xdiff2 += xdiff * xdiff
            ydiff2 += ydiff * ydiff

return diffprod / math.sqrt(xdiff2 * ydiff2)
```

RS have traditionally used either the cosine similarity (Eq. 2.4) or the Pearson correlation (Eq. 2.5)

