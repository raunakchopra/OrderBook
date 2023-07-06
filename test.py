file_labels = ['AAPL','BTC','ETH','IBM','GOOG', 'MSFT','TSLA','TRADES']

test_case = True

for file_label in file_labels:
    chunk_size = 1024 # 1KB
    with open(file_label+'.txt', 'rb') as file1, open('./test_files/'+file_label+'_test.txt', 'rb') as file2:
        while True:
            chunk1 = file1.read(chunk_size)
            chunk2 = file2.read(chunk_size)
            if chunk1 != chunk2:
                test_case = False
                break
            if not chunk1:
                break
if test_case:
    print("Test Case Passed!")
else: 
    print("Test Case Failed")
