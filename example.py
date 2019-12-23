import pandas as pd
import trackC
import numpy as np

data = pd.read_csv('/Users/huwenhao/Documents/workSpace/dataset/clustering/不良驾驶行为数据/超速数据.csv')
data = data[['lng', 'lat']].values[:49999, :]
print("数据量: ", len(data))
model = trackC.Model(1)
res, bounds = model.fit_predict(data)


result = []
for i in range(len(data)):
    result.append({'LngLat': str(data[i][0]) + ',' + str(data[i][1]), 'cluster': res[i]})
result = pd.DataFrame(result)
result.to_csv('/Users/huwenhao/Documents/workSpace/dataset/clustering/聚类结果/超速_聚类结果_点.csv', index=None)
print("聚类结果存放至: /Users/huwenhao/Documents/workSpace/dataset/clustering/聚类结果/超速_聚类结果_点.csv")


result = []
for i in bounds:
    bound = '[' + str(i[0][0]) + ',' + str(i[0][1]) + ']' + ','
    bound = bound + '[' + str(i[1][0]) + ',' + str(i[1][1]) + ']' + ','
    bound = bound + '[' + str(i[2][0]) + ',' + str(i[2][1]) + ']' + ','
    bound = bound + '[' + str(i[3][0]) + ',' + str(i[3][1]) + ']'
    result.append({'bounds': bound})
result = pd.DataFrame(result)
result.to_csv('/Users/huwenhao/Documents/workSpace/dataset/clustering/聚类结果/超速_聚类结果_边界.csv', index=None)
print("边界结果存放至: ./Users/huwenhao/Documents/workSpace/dataset/clustering/聚类结果/超速_聚类结果_边界.csv")

