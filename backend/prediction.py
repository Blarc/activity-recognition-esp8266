import pandas as pd
from joblib import dump
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import KFold, ShuffleSplit

df1 = pd.read_csv('data/data-31_05_2021_12_01_16-walking_3.csv')
df2 = pd.read_csv('data/data-31_05_2021_12_03_17-jumping_3.csv')
df3 = pd.read_csv('data/data-31_05_2021_11_59_23-stillness_3.csv')

df = pd.concat([df1, df2, df3])
df['type'].replace({'walking': 0, 'jumping': 1, 'stillness': 2}, inplace=True)

# X_train, X_test, y_train, y_test = train_test_split(df.iloc[:, :-1], df.iloc[:, -1], test_size=0.1, random_state=0)
X, Y = df.iloc[:, :-1], df.iloc[:, -1]

kfold_cv = KFold(n_splits=10, random_state=0, shuffle=True)
shuffle_cv = ShuffleSplit(n_splits=10, test_size=0.3, random_state=0)

model = RandomForestClassifier()
model.fit(X, Y)

dump(model, 'model.joblib')
