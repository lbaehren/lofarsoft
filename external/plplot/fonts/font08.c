/* $Id: font08.c 2 1992-05-20 21:36:02Z furnish $
   $Log$
   Revision 1.1  1992/05/20 21:33:28  furnish
   Initial checkin of the whole PLPLOT project.

*/

      short int subbuffer175[100] = {
       7748, 8774,   64, 7747, 8773,   64, 7739, 8765,   64, 7738,
       8764,    0,  -16, 7116, 7240, 7760, 7739,   64, 7748, 8134,
       8518, 8773, 8899, 8897, 8767, 8381, 8124, 7739,   64, 7748,
       8133, 8517, 8772,   64, 8645, 8771, 8769, 8639, 8381,    0,
        -16, 7116, 6605, 6985, 6982,   64, 9545, 9542,   64, 6985,
       9545,   64, 6984, 9544,   64, 6983, 9543,   64, 6982, 9542,
          0,  -16, 7116, 7240, 7620, 7617,   64, 8900, 8897,   64,
       7620, 8900,   64, 7619, 8899,   64, 7618, 8898,   64, 7617,
       8897,    0,  -16, 7116, 7240, 7622, 8890,   64, 7622, 7876,
       8131, 8515, 8772, 8901, 8903, 8647, 8645, 8515,   64, 7876
      };
      short int subbuffer176[100] = {
       8515,   64, 8131, 8901,   64, 8775, 8772,   64, 8646, 8902,
         64, 8890, 8636, 8381, 7997, 7740, 7611, 7609, 7865, 7867,
       7997,   64, 8636, 7997,   64, 8381, 7611,   64, 7740, 7737,
         64, 7610, 7866,    0,  -16, 7116, 7240, 8003, 7877, 7879,
       7623, 7621, 7748, 8003, 8387, 8644, 8902,   64, 7751, 7748,
         64, 7622, 7878,   64, 7621, 8387,   64, 8003, 8644,   64,
       8902, 8889,    0,  -16, 7116, 6092, 6828, 6956, 7085, 7086,
       6959, 6831, 6702, 6700, 6826, 7081, 7337, 7722, 7980, 8110,
       8242, 8253, 8151, 8158, 8291, 8421, 8678, 8806, 9061, 9187,
       9183, 9052, 8922, 8663, 8019, 7247, 6989, 6730, 6600, 6468
      };
      short int subbuffer177[100] = {
       6464, 6588, 6841, 7223, 7734, 8246, 8759, 9016, 9275, 9406,
       9410, 9285, 9159, 8905, 8522, 8010, 7625, 7367, 7236, 7232,
       7357, 7611,   64, 6830, 6829, 6957, 6958, 6830,   64, 8663,
       8147, 7503, 7116, 6857, 6727, 6596, 6592, 6716, 6842, 7223,
         64, 8246, 8631, 8888, 9147, 9278, 9282, 9157, 9031, 8777,
       8522,    0,  -16, 7116, 6612, 7743, 7869, 8124, 8380, 8637,
       8767, 8769, 8643, 8388, 8132, 7875, 7746, 7615, 7612, 7737,
       7991, 8374, 8758, 9143, 9401, 9531, 9662, 9666, 9541, 9288,
       9033, 8650, 8266, 7881, 7624, 7366, 7107, 6975, 6970, 7093,
       7345, 7599, 7981, 8492, 9132, 9645,10031,10289,   64, 7366
      };
      short int subbuffer178[100] = {
       7236, 7104, 7098, 7222, 7474, 7728, 8110, 8621, 9133, 9646,
       9903,10289,   64, 8003, 8515,   64, 7874, 8642,   64, 7745,
       8769,   64, 7744, 8768,   64, 7743, 8767,   64, 7870, 8638,
         64, 7997, 8509,   64,10182,10180,10436,10438,10182,   64,
      10310,10308,   64,10181,10437,   64,10172,10170,10426,10428,
      10172,   64,10300,10298,   64,10171,10427,    0,  -16, 7116,
       6478, 6994, 6958,   64, 7634, 7598,   64, 8914, 8878,   64,
       9554, 9518,   64, 7621, 8903,   64, 7620, 8902,   64, 7619,
       8901,   64, 7611, 8893,   64, 7610, 8892,   64, 7609, 8891,
          0,  -16, 7116, 7748, 8129, 8127, 8383, 8385, 8129,   64
      };
      short int subbuffer179[100] = {
       8129, 8383,   64, 8385, 8127,    0,  -16, 7116, 7240, 7618,
       8128, 8510, 8764, 8889, 8887, 8757, 8628,   64, 7617, 8382,
         64, 7616, 7999, 8509, 8763, 8889,    0,  -16, 7116, 7240,
       8903, 8773, 8515, 8001, 7616,   64, 8386, 7615,   64, 8652,
       8779, 8905, 8903, 8772, 8514, 8128, 7614,    0,  -16, 7116,
       6986, 8005, 7620, 7491, 7361, 7359, 7485, 7612, 7995, 8507,
       8892, 9021, 9151, 9153, 9027, 8900, 8517, 8005,   64, 7620,
       7490, 7487, 7613, 7740, 7995,   64, 8892, 9022, 9025, 8899,
       8772, 8517,    0,  -16, 7116, 6986, 8389, 7876, 7490, 7360,
       7358, 7484, 7739, 8123, 8636, 9022, 9152, 9154, 9028, 8773
      };
      short int subbuffer180[100] = {
       8389,   64, 9028, 8389,   64, 8773, 8132, 7490,   64, 7876,
       7360,   64, 7484, 8123,   64, 7739, 8380, 9022,   64, 8636,
       9152,    0,  -16, 7116, 7241, 8389, 8004, 7746, 7616, 7614,
       7740, 7995, 8251, 8636, 8894, 9024, 9026, 8900, 8645, 8389,
         64, 7874, 8645,   64, 7744, 8772,   64, 7614, 8899,   64,
       7741, 9026,   64, 7868, 8896,   64, 7995, 8766,    0,  -16,
       7116, 7240, 7883, 7860,   64, 8652, 8629,   64, 7620, 8902,
         64, 7619, 8901,   64, 7611, 8893,   64, 7610, 8892,    0,
        -16, 7116, 7240, 7756, 7738,   64, 8774, 8756,   64, 7748,
       8774,   64, 7747, 8773,   64, 7739, 8765,   64, 7738, 8764
      };
      short int subbuffer181[100] = {
          0,  -16, 7116, 7240, 7760, 7739,   64, 7748, 8134, 8518,
       8773, 8899, 8897, 8767, 8381, 8124, 7739,   64, 7748, 8133,
       8517, 8772,   64, 8645, 8771, 8769, 8639, 8381,    0,  -16,
       7116, 6605, 6985, 6982,   64, 9545, 9542,   64, 6985, 9545,
         64, 6984, 9544,   64, 6983, 9543,   64, 6982, 9542,    0,
        -16, 7116, 7240, 7620, 7617,   64, 8900, 8897,   64, 7620,
       8900,   64, 7619, 8899,   64, 7618, 8898,   64, 7617, 8897,
          0,  -16, 7116, 7240, 8143, 8773, 8254, 8253,   64, 8646,
       8127,   64, 8521, 8519, 8000, 8253, 8633,   64, 8886, 8633,
       8378, 8122, 7865, 7735, 7733, 7859, 8241,   64, 8886, 8632
      };
      short int subbuffer182[100] = {
       8377, 7865, 7861, 7987, 8241,   64, 8378, 7992, 7733,    0,
        -16, 7116, 7240, 8003, 7877, 7879, 7623, 7621, 7748, 8003,
       8387, 8644, 8902,   64, 7751, 7748,   64, 7622, 7878,   64,
       7621, 8387,   64, 8003, 8644,   64, 8902, 8377,    0,  -16,
       7116, 6092, 6828, 6956, 7085, 7086, 6959, 6831, 6702, 6700,
       6826, 7081, 7337, 7722, 7980, 8110, 8242, 8253, 8151, 8158,
       8291, 8421, 8678, 8806, 9061, 9187, 9183, 9052, 8922, 8663,
       8019, 7247, 6989, 6730, 6600, 6468, 6464, 6588, 6841, 7223,
       7734, 8246, 8759, 9016, 9275, 9406, 9410, 9285, 9159, 8905,
       8522, 8010, 7625, 7367, 7236, 7232, 7357, 7611,   64, 6830
      };
      short int subbuffer183[100] = {
       6829, 6957, 6958, 6830,   64, 8663, 8147, 7503, 7116, 6857,
       6727, 6596, 6592, 6716, 6842, 7223,   64, 8246, 8631, 8888,
       9147, 9278, 9282, 9157, 9031, 8777, 8522,    0,  -16, 7116,
       7128, 7745, 7875, 8132, 8388, 8643, 8769, 8767, 8637, 8380,
       8124, 7869, 7742, 7617, 7620, 7751, 8009, 8394, 8906, 9417,
       9799,10052,10176,10171,10039, 9909, 9650, 9263, 8748, 8105,
       7591,   64, 8906, 9289, 9671, 9924,10048,10043, 9911, 9781,
       9522, 9135, 8491, 8105,   64, 8003, 8515,   64, 7874, 8642,
         64, 7745, 8769,   64, 7744, 8768,   64, 7743, 8767,   64,
       7870, 8638,   64, 7997, 8509,   64,10694,10692,10948,10950
      };
      short int subbuffer184[100] = {
      10694,   64,10822,10820,   64,10693,10949,   64,10684,10682,
      10938,10940,10684,   64,10812,10810,   64,10683,10939,    0,
        -16, 7116, 6478, 6996, 6956,   64, 7124, 7084,   64, 7636,
       7596,   64, 8144, 8400, 8398, 8142, 8145, 8275, 8532, 8916,
       9171, 9425, 9550, 9545, 9414, 9156, 8899, 8643, 8388, 8262,
       8132, 7873, 7744, 7871, 8124, 8250, 8380, 8637, 8893, 9148,
       9402, 9527, 9522, 9391, 9133, 8876, 8492, 8237, 8111, 8114,
       8370, 8368, 8112,   64, 8272, 8270,   64, 8143, 8399,   64,
       9171, 9297, 9422, 9417, 9286, 9156,   64, 8262, 8260, 8001,
       7744, 7999, 8252, 8250,   64, 9148, 9274, 9399, 9394, 9263
      };
      short int subbuffer185[100] = {
       9133,   64, 8242, 8240,   64, 8113, 8369,    0,  -16, 7116,
       6097, 6992, 6960,   64, 7120, 7088,   64, 9424, 9392,   64,
       9552, 9520,   64, 6480,10064,   64, 6448, 7600,   64, 8880,
      10032,    0,  -16, 7116, 6223, 6864, 8130, 6704,   64, 6736,
       8002,   64, 6608, 8001,   64, 6608, 9552, 9801, 9424,   64,
       6833, 9521,   64, 6704, 9520, 9783, 9392,    0,  -16, 7116,
       7113, 9063, 8673, 8282, 8021, 7889, 7756, 7620, 7612, 7732,
       7855, 7979, 8230, 8607, 8985,   64, 8673, 8412, 8150, 8018,
       7884, 7748, 7740, 7860, 7982, 8106, 8356, 8607,    0,  -16,
       7116, 7113, 7527, 7905, 8282, 8533, 8657, 8780, 8900, 8892
      };
      short int subbuffer186[100] = {
       8756, 8623, 8491, 8230, 7839, 7449,   64, 7905, 8156, 8406,
       8530, 8652, 8772, 8764, 8628, 8494, 8362, 8100, 7839,    0,
        -16, 7116, 7113, 7655, 7616, 7577,   64, 7783, 7744, 7705,
         64, 7655, 9063,   64, 7577, 8985,    0,  -16, 7116, 7113,
       8807, 8768, 8729,   64, 8935, 8896, 8857,   64, 7527, 8935,
         64, 7449, 8857,    0,  -16, 7116, 6985, 8807, 8289, 8029,
       7896, 7892, 8016, 8520, 8518, 8388, 8259, 7616, 8253, 8380,
       8506, 8504, 7984, 7852, 7848, 7971, 8223, 8729,   64, 8548,
       8288, 8157, 8024, 8020, 8143, 8268,   64, 8244, 8113, 7980,
       7976, 8099, 8224, 8476,    0,  -16, 7116, 7114, 7783, 8289
      };
      short int subbuffer187[100] = {
       8541, 8664, 8660, 8528, 8008, 8006, 8132, 8259, 8896, 8253,
       8124, 7994, 7992, 8496, 8620, 8616, 8483, 8223, 7705,   64,
       8036, 8288, 8413, 8536, 8532, 8399, 8268,   64, 8244, 8369,
       8492, 8488, 8355, 8224, 7964,    0,  -16, 7116, 7113, 8804,
       8417, 8158, 7898, 7765, 7759, 7881, 8005, 8378, 8502, 8624,
       8619, 8486, 8355, 8095,   64, 8417, 8157, 8026, 7893, 7888,
       8010, 8134, 8507, 8631, 8753, 8747, 8614, 8354, 8095, 7708,
          0,  -16, 7116, 7113, 7780, 8161, 8414, 8666, 8789, 8783,
       8649, 8517, 8122, 7990, 7856, 7851, 7974, 8099, 8351,   64,
       8161, 8413, 8538, 8661, 8656, 8522, 8390, 7995, 7863, 7729
      };
      short int subbuffer188[100] = {
       7723, 7846, 8098, 8351, 8732,    0,  -16, 7116, 4808, 5184,
       6080, 8227,   64, 5952, 8099,   64, 5824, 8224,   64, 9328,
       8776, 8224,    0,  -16, 7116, 6351, 9700, 9572, 9443, 9442,
       9569, 9697, 9826, 9828, 9702, 9447, 9191, 8934, 8676, 8546,
       8415, 8280, 8136, 8104, 7967, 7836,   64, 9571, 9570, 9698,
       9699, 9571,   64, 8280, 8232,   64, 8676, 8545, 8408, 8376,
       8232, 8097, 7966, 7836, 7578, 7321, 7065, 6810, 6684, 6686,
       6815, 6943, 7070, 7069, 6940, 6812,   64, 6814, 6813, 6941,
       6942, 6814,    0,  -16, 7116, 6986, 8268, 7223,   64, 8265,
       7351, 7223,   64, 8265, 9143, 9271,   64, 8268, 9271,   64
      };
      short int subbuffer189[100] = {
       7613, 8893,   64, 7484, 9020,    0,  -16, 7116, 6986, 7500,
       7479,   64, 7627, 7608,   64, 7500, 8524, 8907, 9034, 9160,
       9157, 9027, 8898, 8513,   64, 7627, 8523, 8906, 9032, 9029,
       8899, 8514,   64, 7618, 8514, 8897, 9024, 9150, 9147, 9017,
       8888, 8503, 7479,   64, 7617, 8513, 8896, 9022, 9019, 8889,
       8504, 7608,    0,  -16, 7116, 6987, 9287, 9161, 8907, 8652,
       8140, 7883, 7625, 7495, 7364, 7359, 7484, 7610, 7864, 8119,
       8631, 8888, 9146, 9276,   64, 9287, 9159, 9033, 8906, 8651,
       8139, 7882, 7623, 7492, 7487, 7612, 7865, 8120, 8632, 8889,
       9018, 9148, 9276,    0,  -16, 7116, 6987, 7500, 7479,   64
      };
      short int subbuffer190[100] = {
       7627, 7608,   64, 7500, 8396, 8779, 9033, 9159, 9284, 9279,
       9148, 9018, 8760, 8375, 7479,   64, 7627, 8395, 8778, 8905,
       9031, 9156, 9151, 9020, 8890, 8761, 8376, 7608,    0,  -16,
       7116, 7114, 7628, 7607,   64, 7755, 7736,   64, 7628, 9164,
         64, 7755, 9163, 9164,   64, 7746, 8514, 8513,   64, 7745,
       8513,   64, 7736, 9144, 9143,   64, 7607, 9143,    0,  -16,
       7116, 7113, 7628, 7607,   64, 7755, 7735, 7607,   64, 7628,
       9164,   64, 7755, 9163, 9164,   64, 7746, 8514, 8513,   64,
       7745, 8513,    0,  -16, 7116, 6987, 9287, 9161, 8907, 8652,
       8140, 7883, 7625, 7495, 7364, 7359, 7484, 7610, 7864, 8119
      };
      short int subbuffer191[100] = {
       8631, 8888, 9146, 9276, 9280, 8640,   64, 9287, 9159, 9033,
       8906, 8651, 8139, 7882, 7753, 7623, 7492, 7487, 7612, 7738,
       7865, 8120, 8632, 8889, 9018, 9148, 9151, 8639, 8640,    0,
        -16, 7116, 6859, 7372, 7351,   64, 7372, 7500, 7479, 7351,
         64, 9164, 9036, 9015, 9143,   64, 9164, 9143,   64, 7490,
       9026,   64, 7489, 9025,    0,  -16, 7116, 7749, 8268, 8247,
       8375,   64, 8268, 8396, 8375,    0,  -16, 7116, 7241, 8780,
       8764, 8633, 8376, 8120, 7865, 7740, 7612,   64, 8780, 8908,
       8892, 8761, 8632, 8375, 8119, 7864, 7737, 7612,    0,  -16,
       7116, 6987, 7500, 7479, 7607,   64, 7500, 7628, 7607,   64
      };
      short int subbuffer192[100] = {
       9292, 9164, 7616,   64, 9292, 7615,   64, 8003, 9143, 9271,
         64, 8131, 9271,    0,  -16, 7116, 7112, 7628, 7607,   64,
       7628, 7756, 7736,   64, 7736, 9144, 9143,   64, 7607, 9143,
          0,  -16, 7116, 6732, 7244, 7223,   64, 7367, 7351, 7223,
         64, 7367, 8247,   64, 7244, 8250,   64, 9292, 8250,   64,
       9159, 8247,   64, 9159, 9143, 9271,   64, 9292, 9271,    0,
        -16, 7116, 6859, 7372, 7351,   64, 7497, 7479, 7351,   64,
       7497, 9143,   64, 7372, 9018,   64, 9036, 9018,   64, 9036,
       9164, 9143,    0,  -16, 7116, 6859, 8012, 7755, 7497, 7367,
       7236, 7231, 7356, 7482, 7736, 7991, 8503, 8760, 9018, 9148
      };
      short int subbuffer193[100] = {
       9279, 9284, 9159, 9033, 8779, 8524, 8012,   64, 8139, 7754,
       7495, 7364, 7359, 7484, 7737, 8120, 8376, 8761, 9020, 9151,
       9156, 9031, 8778, 8395, 8139,    0,  -16, 7116, 6986, 7500,
       7479,   64, 7627, 7607, 7479,   64, 7500, 8652, 8907, 9034,
       9160, 9157, 9027, 8898, 8641, 7617,   64, 7627, 8651, 8906,
       9032, 9029, 8899, 8642, 7618,    0,  -16, 7116, 6859, 8012,
       7755, 7497, 7367, 7236, 7231, 7356, 7482, 7736, 7991, 8503,
       8760, 9018, 9148, 9279, 9284, 9159, 9033, 8779, 8524, 8012,
         64, 8139, 7754, 7495, 7364, 7359, 7484, 7737, 8120, 8376,
       8761, 9020, 9151, 9156, 9031, 8778, 8395, 8139,   64, 8378
      };
      short int subbuffer194[100] = {
       9013, 9141,   64, 8378, 8506, 9141,    0,  -16, 7116, 6986,
       7500, 7479,   64, 7627, 7607, 7479,   64, 7500, 8524, 8907,
       9034, 9160, 9157, 9027, 8898, 8513, 7617,   64, 7627, 8523,
       8906, 9032, 9029, 8899, 8514, 7618,   64, 8257, 9015, 9143,
         64, 8385, 9143,    0,  -16, 7116, 6986, 9161, 8907, 8524,
       8012, 7627, 7369, 7367, 7493, 7620, 7875, 8513, 8768, 8895,
       9021, 9018, 8889, 8504, 7992, 7737, 7610, 7354,   64, 9161,
       8905, 8778, 8523, 8011, 7626, 7497, 7495, 7621, 7876, 8514,
       8769, 9023, 9149, 9146, 8888, 8503, 7991, 7608, 7354,    0,
        -16, 7116, 7241, 8267, 8247,   64, 8395, 8375, 8247,   64
      };
      short int subbuffer195[100] = {
       7500, 9164, 9163,   64, 7500, 7499, 9163,    0,  -16, 7116,
       6859, 7372, 7357, 7482, 7736, 8119, 8375, 8760, 9018, 9149,
       9164,   64, 7372, 7500, 7485, 7610, 7737, 8120, 8376, 8761,
       8890, 9021, 9036, 9164,    0,  -16, 7116, 6986, 7244, 8247,
         64, 7244, 7372, 8250,   64, 9292, 9164, 8250,   64, 9292,
       8247,    0,  -16, 7116, 6605, 6860, 7607,   64, 6860, 6988,
       7610,   64, 8268, 7610,   64, 8265, 7607,   64, 8265, 8887,
         64, 8268, 8890,   64, 9676, 9548, 8890,   64, 9676, 8887,
          0,  -16, 7116, 6986, 7372, 9015, 9143,   64, 7372, 7500,
       9143,   64, 9164, 9036, 7351,   64, 9164, 7479, 7351,    0
      };
      short int subbuffer196[100] = {
        -16, 7116, 7114, 7372, 8258, 8247, 8375,   64, 7372, 7500,
       8386,   64, 9292, 9164, 8258,   64, 9292, 8386, 8375,    0,
        -16, 7116, 6986, 9036, 7351,   64, 9164, 7479,   64, 7372,
       9164,   64, 7372, 7371, 9035,   64, 7480, 9144, 9143,   64,
       7351, 9143,    0,  -16, 7116, 6602, 9036, 8778, 8519, 8130,
       7871, 7483, 7096, 6839, 6583, 6456, 6458, 6587, 6714, 6585,
         64, 9036, 8904, 8638, 8503,   64, 9036, 8631,   64, 8503,
       8505, 8380, 8254, 8000, 7745, 7489, 7360, 7358, 7483, 7864,
       8247, 8759, 9016,    0,  -16, 7116, 6732, 8651, 8522, 8392,
       8131, 7869, 7739, 7480, 7223,   64, 8522, 8391, 8127, 7996
      };
      short int subbuffer197[100] = {
       7866, 7608, 7223, 6967, 6840, 6842, 6971, 7098, 6969,   64,
       7878, 7748, 7619, 7363, 7236, 7238, 7368, 7626, 7883, 8268,
       9036, 9291, 9417, 9415, 9285, 9028, 8515, 8259,   64, 9036,
       9163, 9289, 9287, 9157, 9028,   64, 8515, 8898, 9025, 9151,
       9148, 9017, 8888, 8631, 8375, 8248, 8250, 8381,   64, 8515,
       8770, 8897, 9023, 9020, 8889, 8631,    0,  -16, 7116, 6987,
       7370, 7240, 7238, 7364, 7747, 8131, 8644, 8901, 9159, 9289,
       9291, 9164, 8908, 8523, 8136, 7877, 7617, 7485, 7482, 7608,
       7991, 8247, 8632, 8890, 9020, 9022, 8896, 8640, 8383, 8253,
         64, 8908, 8651, 8264, 8005, 7745, 7613, 7610, 7736, 7991
      };
      short int subbuffer198[100] = {
          0,  -16, 7116, 6731, 8651, 8522, 8392, 8131, 7869, 7739,
       7480, 7223,   64, 8522, 8391, 8127, 7996, 7866, 7608, 7223,
       6967, 6840, 6842, 6971, 7227, 7482, 7736, 7991, 8375, 8632,
       8890, 9150, 9283, 9286, 9161, 8907, 8652, 8012, 7627, 7369,
       7239, 7237, 7364, 7620, 7749, 7879,    0,  -16, 7116, 7114,
       8905, 8776, 8774, 8901, 9157, 9287, 9289, 9163, 8908, 8524,
       8267, 8138, 8008, 8006, 8132, 8387,   64, 8524, 8266, 8136,
       8133, 8387,   64, 8387, 8131, 7746, 7488, 7358, 7355, 7481,
       7608, 7863, 8247, 8632, 8890, 9020, 9022, 8896, 8640, 8383,
       8253,   64, 8131, 7874, 7616, 7486, 7482, 7608,    0,  -16
      };
      short int subbuffer199[100] = {
       7116, 6858, 8906, 8776, 8515, 8253, 8123, 7864, 7607,   64,
       8134, 8004, 7747, 7491, 7365, 7367, 7497, 7755, 8140, 9420,
       9035, 8906, 8775, 8511, 8380, 8250, 7992, 7607, 7351, 7096,
       6969, 6970, 7099, 7226, 7097,   64, 8396, 8907, 9035,   64,
       7871, 8000, 8257, 8769, 9026, 9285, 9022,    0,  -16, 7116,
       6859, 7241, 7111, 7109, 7235, 7490, 7874, 8259, 8516, 8903,
       9034, 9035, 8908, 8780, 8523, 8265, 8135, 8004, 8001, 8127,
       8382, 8638, 8895, 9153, 9283,   64, 8908, 8651, 8393, 8263,
       8132, 8128, 8382,   64, 9283, 9151, 8891, 8633, 8376, 7863,
       7479, 7224, 7098, 7099, 7228, 7355, 7226,   64, 9151, 8892
      };
