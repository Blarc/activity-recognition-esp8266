import pandas as pd
from joblib import dump
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import KFold, ShuffleSplit

df1 = pd.read_csv('data/data-27_05_2021_16_26_46-luka_2_walking_horizontal.csv')
df2 = pd.read_csv('data/data-27_05_2021_16_29_59-jakob_2_walking_horizontal.csv')
df3 = pd.read_csv('data/data-27_05_2021_16_33_54-jakob_2_jumping_horizontal.csv')
df4 = pd.read_csv('data/data-27_05_2021_16_38_13-luka_2_jumping_horizontal.csv')

df = pd.concat([df1, df2, df3, df4])
df['type'].replace({'walking': 0, 'jumping': 1}, inplace=True)

# X_train, X_test, y_train, y_test = train_test_split(df.iloc[:, :-1], df.iloc[:, -1], test_size=0.1, random_state=0)
X, Y = df.iloc[:, :-1], df.iloc[:, -1]

kfold_cv = KFold(n_splits=10, random_state=0, shuffle=True)
shuffle_cv = ShuffleSplit(n_splits=10, test_size=0.3, random_state=0)

model = RandomForestClassifier()
model.fit(X, Y)

dump(model, 'model.joblib')
